import nacl.secret
import nacl.utils
import boto3
import os


                                                                                             #verbind met de Backblaze S3
s3_client = boto3.client('s3', 
                aws_access_key_id="0123456789",                                              #keyID van Backblaze, vervang door daadwerkelijke keyID  
                aws_secret_access_key="abcdefghijklmnopqrstuvqxyz",                          #private key van Backblaze, vervang door daadwerkelijke private key
                endpoint_url="https://endpoint.com")                                         #locatie van Backblaze, vervang door daadwerkelijke private key 


def encrypt_and_fragment(input_file):

    bucket1 = 'name_of_the_first_bucket'                                                     #naam van de eerste bucket, vervang door daadwerkelijke naam
    bucket2 = 'name_of_the_second_bucket'                                                    #naam van de eerste bucket, vervang door daadwerkelijke naam
    bucket3 = 'name_of_the_third_bucket'                                                     #naam van de eerste bucket, vervang door daadwerkelijke naam

    key = nacl.utils.random(nacl.secret.SecretBox.KEY_SIZE)                                  #maak een key aan voor het bestand dat ge-encrypt moet worden
    box = nacl.secret.SecretBox(key)                                                         #maak een box (een kluis) aan met de gemaakte key

    key_file = open('key_encrypted_' + input_file, "wb")                                     #maak bestand voor eigen sleutel onder eigen naam
    key_file.write(key)                                                                      #zet de inhoud van key in sleutel bestand

    file = open(input_file, "rb")                                                            #open het ingevoerde bestand als binair
    binary_data = file.read()                                                                #zet inhoud van het bestand in variabele 'binary_data'

    encrypted_data = box.encrypt(binary_data)                                                #encrypt de inhoud van het bestand, dit wordt gedaan met de gemaakte box
    print("encrytion of file complete")
    block_length = len(encrypted_data) // 3                                                  #bereken de lengte van de blokken door de lengte van het bestand door 3 te delen
    block1 = encrypted_data[:block_length]                                                   #block1 is de eerste 1/3 deel  van het bestand
    block2 = encrypted_data[block_length:2 * block_length]                                   #block1 is de tweede 1/3 deel van het bestand
    block3 = encrypted_data[2 * block_length:]                                               #block3 is de derde 1/3 deel van het bestand
    print("fragmentation of file complete")

    upload_file(block1, bucket1, 'first_fragment_' + input_file)                             #eerste fragement wordt gemaakt met inhoud van block1, komt in eigen 'first_fragment' bestand
    upload_file(block2, bucket2, 'second_fragment_' + input_file)                            #tweede fragement wordt gemaakt met inhoud van block2, komt in eigen 'second_fragment' bestand
    upload_file(block3, bucket3, 'third_fragment_' + input_file)                             #derde fragement wordt gemaakt met inhoud van block3, komt in eigen 'third_fragment' bestand

def upload_file(data, bucket_name, file_name):                                               #functie voor het uploaden van een bestand naar de cloud
    s3_client.put_object(Bucket=bucket_name, Key=file_name, Body=data)                       #put_object accepteerd geen bestanden, maar maakt in backblaze een bestand aan, hier komt d einhoud van 'data' in
    print("upload of "+file_name+ " is a success")


def download_file(bucket, object_name, download_file_name):                                  #functie voor het downloaden van een bestand naar de cloud
    if object_name is None:                                                                  #object_name is hetzelfde als download_file_name, dus het input bestand
        object_name = os.path.basename(download_file_name)
    
    s3_client.download_file(bucket, object_name, download_file_name)                         #download een bestand uit de jusite bucket
    print(download_file_name + " has been downloaded from bucket " + bucket)


def download_decrypt_combine(output_file):
    bucket1 = 'name_of_the_first_bucket'                                                     #naam van de eerste bucket, vervang door daadwerkelijke naam
    bucket2 = 'name_of_the_second_bucket'                                                    #naam van de eerste bucket, vervang door daadwerkelijke naam
    bucket3 = 'name_of_the_third_bucket'                                                     #naam van de eerste bucket, vervang door daadwerkelijke naam
    download_file(bucket1, None,  'first_fragment_' + output_file)                           #download het eerste frament uit bucket 1
    download_file(bucket2, None, 'second_fragment_' + output_file)                           #download het tweede fragment uit bucket 2
    download_file(bucket3, None,  'third_fragment_' + output_file)                           #download het derde fragment uit bucket 3
    combine_and_decrypt(output_file)                                                         #voeg alle blokken samen en decrypt deze met de functie


def combine_and_decrypt(output_file):
    with open('first_fragment_' + output_file, "rb") as file1:                               #open bestand voor eerste fragment
        block1 = file1.read()                                                                #zet de inhoud van fragment in block1
    with open('second_fragment_' + output_file, "rb") as file2:                              #open bestand voor tweede fragment
        block2 = file2.read()                                                                #zet de inhoud van fragment in block2
    with open('third_fragment_' + output_file, "rb") as file3:                               #open bestand voor derde fragment
        block3 = file3.read()                                                                #zet de inhoud van fragment in block3
       
    encrypted_data = block1 + block2 + block3                                                #voeg de drie blokken samen door ze achter elkaar te plakken

    key_file = open('key_encrypted_' + output_file, "rb")                                    #open het bestand van de sleutel van het ingegeven bestand
    key = key_file.read()                                                                    #lees het sleutelbestand en zet in variabele 'key'

    box = nacl.secret.SecretBox(key)                                                         #maak box variabele aan door de sleutel te gebruiken uit het bestand
    plaintext = box.decrypt(encrypted_data)                                                  #decrpyt de inhoud van encrypted_data (samengevoegde blokken) met de box                           
    print("Decrypted message is:")
    print(plaintext.decode('utf-8'))                                                         #print de gedecrypte inhoud en zet in utf-8 (ascii)
    
    os.remove('first_fragment_' + output_file)                                               #verwijder het bestand voor blok 1
    os.remove('second_fragment_' + output_file)                                              #verwijder het bestand voor blok 2
    os.remove('third_fragment_' + output_file)                                               #verwijder het bestand voor blok 3



choice = input('Enter 1 to upload or 2 to decrypt: ')                                        #keuze maken wat gebruiker wil doen

if choice == "1":
    input_file = input("Enter filename to upload: ")                        
    encrypt_and_fragment(input_file)                                                         #roep encrypt_fragment_upload aan en geen het ingegeven bestand mee
elif choice == "2":
    output_file = input("Enter filename to download: ")
    download_decrypt_combine(output_file)                                                    #roep download_decrypt aan en geef ingegevn bestand mee                                                         
else:
    print("error")

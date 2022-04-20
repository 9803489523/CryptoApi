#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
void CharToByte(char* chars, byte* bytes, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		bytes[i] = (byte)chars[i];
		//cout << chars[i];
	}
}
void ByteToChar(byte* bytes, char* chars, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		chars[i] = (char)bytes[i];
		//cout << chars[i];
	}
}
void pause() {
	system("pause");
}
int main() {
	cout << "Portnaya\n";
	setlocale(LC_ALL, "Russian");
	HCRYPTPROV prov;
	HCRYPTKEY ses_key;
	CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0);
	if (CryptGenKey(prov, CALG_RC4, CRYPT_EXPORTABLE, &ses_key)) {
		cout << "CryptGenKey = 1\n";
	}
	else {
		cout << "CryptGenKey = 0\n";
	}
	/* * * * * * * * * * * */
	
		DWORD Open_Blob_len;
	ifstream fin_len("../../Laba5/Blob_len.txt");
	fin_len >> Open_Blob_len;
	fin_len.close();
	/* * * * * * * * * * * */
	byte* OpenBlob = new byte[Open_Blob_len];
	char* readed_file = new char[Open_Blob_len];
	char file_name[] = "../../Laba5/OpenKeyBlob.txt";
	std::ifstream is(file_name, ios::binary);
	is.read(readed_file, Open_Blob_len);
	for (int i = 0; i < Open_Blob_len; i++) {
		//cout << (byte)readed_file[i] << ' ';
	}
	is.close();
	CharToByte(readed_file, OpenBlob, Open_Blob_len);
	HCRYPTKEY open_key;
	if (CryptImportKey(prov, OpenBlob, Open_Blob_len, 0, 0, &open_key)) {
		cout << "\nОткрытый ключ Виль получен\n";
	}
	else {
		cout << "\nОткрытый ключ Виль не был получен\n";
		return(1);
	}
	/* * * * * * * * * * * */
	/* * * * * * * * * * * */
	//Экспорт зашифрованного сессинного ключа
	DWORD pdDataLen;
	if (CryptExportKey(ses_key, open_key, SIMPLEBLOB, 0, 0, &pdDataLen)) {
		cout << "Память под сессионный BLOB выделена\n";
	}
	else {
		cout << "Память под сессионный не BLOB выделена\n";
		
			return(1);
	}
	cout << "Размер сессионого BLOB = " << pdDataLen << endl;
	byte* SessionKeyBlob = new byte[pdDataLen];
	if (CryptExportKey(ses_key, open_key, SIMPLEBLOB, 0, SessionKeyBlob,
		&pdDataLen)) {
		cout << "Сессионный ключ был записан в BLOB\n";
	}
	else {
		cout << "Сессионный ключ НЕ был записан в BLOB\n";
		return(1);
	}
	/* * * * * * * * * * * */
	//Запись зашифрованного сессионного блоба в файл
	ofstream ses_blob;
	ses_blob.open(L"../../Laba5/ses_blob.txt", ios::binary);
	if (ses_blob.is_open())
	{
		for (int i = 0; i < pdDataLen; i++) {
			ses_blob << SessionKeyBlob[i];
			//cout << (byte)SessionKeyBlob[i] << ' ';
		}
	}
	ses_blob.close();
	/* * * * * * * * * * * */
	/* * * * * * * * * * * */
	//Запись РАЗМЕРА зашифрованного сессионного блоба в файл
	ofstream ses_blob_len;
	ses_blob_len.open(L"../../Laba5/ses_blob_len.txt");
	if (ses_blob_len.is_open())
	{
		ses_blob_len << pdDataLen;
	}
	ses_blob_len.close();
	
		/* * * * * * * * * * * */
		cout << "\nОжидаю зашифрованное сообщение от Виль\n\n";
	pause();
	/* * * * * * * * * * * */
	// Чтение текста
	DWORD datalen;
	ifstream fin("../../Laba5/count.txt");
	fin >> datalen;
	fin.close();
	char* char_enc_file = new char[datalen];
	byte* enc_file = new byte[datalen];
	char enc_stream_file_name[] = "../../Laba5/Text.txt";
	std::ifstream enc_stream(enc_stream_file_name); // open file
	enc_stream.read(char_enc_file, datalen);
	for (int i = 0; i < datalen; i++) {
		enc_file[i] = char_enc_file[i];
	}
	enc_stream.close();
	/* * * * * * * * * * * */
	if (CryptDecrypt(ses_key, NULL, true, NULL, enc_file, &datalen)) {
		cout << "CryptDecrypt == 1\n";
	}
	else {
		cout << "\nCryptDecrypt == 0\n";
	}
	ofstream decrypted;
	decrypted.open("../../Laba5/Text.txt");
	for (int i = 0; i < datalen; i++) {
		cout << enc_file[i];
		decrypted << enc_file[i];
	}
	decrypted.close();
}
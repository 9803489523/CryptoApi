#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
void CharToByte(char* chars, byte* bytes, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		bytes[i] = (byte)chars[i];
	}
}
void ByteToChar(byte* bytes, char* chars, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		chars[i] = (char)bytes[i];
	}
}
void DoEncrypt(HCRYPTKEY EncryptKey);
void pause() {
	system("pause");
}
int main() {
	cout << "Vill\n";
	setlocale(LC_ALL, "Russian");
	HCRYPTPROV prov;
	HCRYPTKEY key_pair;
	CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0);
	if (CryptGenKey(prov, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &key_pair)) {
		cout << "Пара ключей сгенерирована!\n";
	}
	if (CryptGetUserKey(prov, AT_KEYEXCHANGE, &key_pair)) {
		cout << "Ключ получен!\n";
	}
	DWORD dwBlobLength;
	
		if (CryptExportKey(key_pair, NULL, PUBLICKEYBLOB, 0, 0, &dwBlobLength)) {
			cout << "Память под открытый BLOB выделена\n";
		}
	byte* OpenBlob = new byte[dwBlobLength];
	if (CryptExportKey(key_pair, NULL, PUBLICKEYBLOB, 0, OpenBlob,
		&dwBlobLength)) {
		cout << "Открытый ключ записан в BLOB\n";
	}
	/* * * * * * */
	ofstream Blob_Write;
	Blob_Write.open(L"../../Laba5/OpenKeyBlob.txt", ios::binary);
	if (Blob_Write.is_open())
	{
		for (int i = 0; i < dwBlobLength; i++) {
			Blob_Write << OpenBlob[i];
			//cout << byte(OpenBlob[i]) << ' ';
		}
	}
	Blob_Write.close();
	/* * * * * * */
	/* * * * * * */
	ofstream Blob_Len;
	Blob_Len.open(L"../../Laba5/Blob_len.txt");
	if (Blob_Len.is_open())
	{
		Blob_Len << dwBlobLength;
	}
	Blob_Len.close();
	/* * * * * * */
	cout << "\nОжидание сессионного ключа Портной!\n";
	pause();
	/* * * * * * * * * * * */
	DWORD ses_blob_len;
	ifstream fin_len("../../Laba5/ses_blob_len.txt");
	fin_len >> ses_blob_len;
	
		cout << "ses_blob_len = " << ses_blob_len << endl;
	fin_len.close();
	/* * * * * * * * * * * */
	char file_name[] = "../../Laba5/ses_blob.txt";
	byte* ses_blob = new byte[ses_blob_len];
	char* readed_file = new char[ses_blob_len];
	std::ifstream is(file_name, ios::binary);
	is.read(readed_file, ses_blob_len);
	for (int i = 0; i < ses_blob_len; i++) {
		//cout << (byte)readed_file[i] << ' ';
	}
	is.close();
	/* * * * * * */
	CharToByte(readed_file, ses_blob, ses_blob_len);
	HCRYPTKEY Portnaya_session_key;
	if (CryptImportKey(prov, ses_blob, ses_blob_len, key_pair, 0, &Portnaya_session_key)) {
		cout << "\nСессионный ключ Портной получен\n";
	}
	else {
		cout << "\nСессионный ключ Портной не был получен\n";
		return(1);
	}
	DoEncrypt(Portnaya_session_key);
	/* * * * * * */
}
/* * * * * * * * * * */
// Шифрование файла //
void DoEncrypt(HCRYPTKEY EncryptKey) {
	char* readed_file = new char[10000];
	int count_chars = 0;
	char file_name[] = "../../Laba5/Text.txt";
	std::ifstream is(file_name);
	char c;
	while (is.get(c)) {
		
			readed_file[count_chars] = c;
		count_chars++;
		cout << c;
	}
	is.close();
	byte* byte_file = new byte[count_chars];
	CharToByte(readed_file, byte_file, count_chars);
	DWORD DataLen = count_chars;
	if (CryptEncrypt(EncryptKey, 0, 1, NULL, byte_file, &DataLen, DataLen)) {
		cout << "\nЗашифровано\n";
	}
	for (int i = 0; i < DataLen; i++) {
		cout << byte_file[i];
	}
	/* * * * * * * * * * */
	ofstream out;
	out.open("../../Laba5/Text.txt");
	if (out.is_open())
	{
		for (int i = 0; i < DataLen; i++) {
			out << byte_file[i];
		}
	}
	out.close();
	/* * * * * * * * * * */
	ofstream cou;
	cou.open("../../Laba5/count.txt");
	if (cou.is_open())
	{
		cou << DataLen;
	}
	cou.close();
}
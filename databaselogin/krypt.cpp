//Krypt by Speedy. Not to be distributed.
//
#include <iostream>
#include <string>
#include <cstring>
#include <math.h>
#include <sstream>
#include <algorithm>

using namespace std;

inline char abc[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

inline char abcbig[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

inline char nbr[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

inline char sym[8] = { '!', '@', '#', '$', '%', '^', '&', '*' };

inline string krypt(string input, bool show_status) {
    if (input == "" || input.empty()) {
        return " ";
    }
    char* chr;
    if (!input.empty() && show_status) {
        cout << "encrypting..." << endl << endl;
    }
    else if (input.empty() && show_status) {
        cout << "error: string is empty!" << endl << endl;
        return "";
    }
    string str_obj(input);
    chr = &str_obj[0];

    int length = input.length();

    int delta = 2 * pow(length - 1, 2);
    int s1, s2, s3;
    s1 = ((sqrt(delta) + length) * length) - (sqrt(length) * 2) + 1;
    s2 = ((sqrt(delta) - length) * length) + (sqrt(length) * 2) - 1;
    s3 = sqrt(pow(sqrt(s1 + s2), 3) + 1);

    while (s3 > 10) {
        s3 -= (s3 / 2) + (s3 % 2);
    }

    unsigned long long int encryptionkey = ((((length * 10 + s3) / length) * sqrt(s3)) + s3 * length + chr[0]) * pow(chr[1], 2) / sqrt(chr[0]);

    int symnbr;
    symnbr = encryptionkey;
    while (symnbr > 8) {
        symnbr /= 8;
    }

    int a = 0;

    for (int i = 1; i <= s3 * 2; i++) {
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < 26; j++) {
                if (chr[i] == abc[j]) {
                    a = j + s3;
                    if (a >= 26) {
                        a = a - 26;
                    }
                    chr[i] = abc[a];
                    break;
                }
                else if (chr[i] == abcbig[j]) {
                    a = j + s3;
                    if (a >= 26) {
                        a = a - 26;
                    }
                    chr[i] = abcbig[a];
                    break;
                }
            }
        }
    }
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 10; j++) {
            if (chr[i] == nbr[j]) {
                a = j + s3;
                if (a >= 10) {
                    a = a - 10;
                }
                chr[i] = nbr[a];
                break;
            }
        }
    }
    //for(int i = 0; i<length; i++){
    //  cout << chr[i];
    //}
    string krypted;
    for (int i = 0; i < length; i++) {
        krypted.push_back(chr[i]);
    }
    auto to_hex = [](string input) -> string {
        int length = input.length();
        stringstream ss;
        for (int i = 0; i < length; i++) {
            ss << hex << (int)input[i];
        }
        return ss.str();
    };
    string hexxed = to_hex(krypted);
    int hexlength = hexxed.length();
    int spd2 = 0;
    transform(hexxed.begin(), hexxed.end(), hexxed.begin(), ::toupper);

    string encstr = to_string(encryptionkey);
    string symbol(1, sym[symnbr]);
    string fullenc = symbol + to_string(encryptionkey);

    string layer3 = hexxed + fullenc;
    if (show_status) {
        cout << layer3;
    }
    return layer3;

    //cout << endl << "encryption key: " << sym[symnbr] << encryptionkey << endl;
    //cout << endl << endl << "If the encryption key is the same as the decryption key that means the message was decrypted succesfully" << endl;
}
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>

using namespace std;

typedef unsigned int u_int;
const char *validTitle[] = {
	"AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", 
	"ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", 
	"MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", 
	"RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB", 
	"TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", 
	"TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2", 
	"TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", 
	"TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", 
	"TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", 
	"TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE", 
	"TYER", "TXXX", "UFID", "USER", "USLT", "WCOM", 
	"WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY", 
	"WPUB", "WXXX", 
	//6 * 12 + 2 = 74
};
const u_int titleNum = 74;

struct FrameHeader {
	char id[4];
	u_int size;
	char flag[2];
	FrameHeader(char *_id, u_int _size, char *_flag) {
		memcpy(id, _id, 4);
		memcpy(flag, _flag, 2);
		size = _size;
	}
	FrameHeader() {}
	void print() const {
		printf("header: id-");
		for(int i = 0; i < 4; ++i) putchar(id[i]);
		printf("\tsize-%u\tflag-%x%x\n", size, flag[0], flag[1]);
	}
	bool isValid() {
		for(u_int i = 0; i < titleNum; ++i) {
			if(strncmp(validTitle[i], id, 4) == 0) return true;
		}
		return false;
	}
};

class Frame {
	FrameHeader header;
	char *data;
	public:
	Frame(const char *_data, FrameHeader _header):header(_header) { 
		data = new char[_header.size];
		if(data == NULL) {
			printf("new %d bytes fail.\n", _header.size);
			exit(0);
		}
		memcpy(data, _data, _header.size);
	}
	~Frame() { if(data != NULL) delete[] data; }
	void setHeader(const FrameHeader& fh) {
		header = fh;
	}
	const FrameHeader& getHeader() { return header; }
	const char* getData() { return data; }
	void print(void) {

		header.print();
		printf("encoding: %x ", data[0]);
		for(u_int i = 0; i < header.size; ++i)
			printf("%c", data[i]);
		putchar('\n');
		/*printf("text encoding: %c \n", data[0]);
		  int end = 1;
		  for(; data[end] != 0; ++end);
		  printf("MIME type: %s \n", data + 1);
		  printf("picture type: %c \n", data[++end]);
		  for(; data[end] != 0; ++end);
		 */
	}
};
struct Header {
	char id[3];
	char version[2];
	char flag;
	u_int size;
};
class ID3v2 {
	public:
		ID3v2(const char *file) { 
			u_int temp = strlen(file);
			fileName = new char[temp + 1];
			memcpy(fileName, file, temp+1);
			mp3 = fopen(file, "rb");
			if(mp3 == NULL) {
				printf("open %s fail.\n", file);
				exit(0);
			}
			deal();
		}
		~ID3v2() { if(mp3 != NULL) fclose(mp3); }
		//????header
		void getHeader(void);
		//????frameHeader
		FrameHeader getFrameHeader(void);
		//????frame
		bool getFrame(void);
		//fread size bytes.
		void read(char *str, u_int size);

		void deal(void);

		//
		void dealTag(Frame& fr);

		void UFID(Frame& fr);
		void TXXX(Frame& fr);
		void WXXX(Frame& fr);
		void IPLS(Frame& fr);
		void MCDI(Frame& fr);
		void ETCO(Frame& fr);
		void MLLT(Frame& fr);
		void SYTC(Frame& fr);
		void USLT(Frame& fr);
		void SYLT(Frame& fr);
		void COMM(Frame& fr);
		void RVAD(Frame& fr);
		void EQUA(Frame& fr);
		void RVRB(Frame& fr);
		void APIC(Frame& fr);
		void GEOB(Frame& fr);
		void PCNT(Frame& fr);
		void POPM(Frame& fr);
		void RBUF(Frame& fr);
		void AENC(Frame& fr);
		void LINK(Frame& fr);
		void POSS(Frame& fr);
		void USER(Frame& fr);
		void OWNE(Frame& fr);
		void COMR(Frame& fr);
		void ENCR(Frame& fr);
		void GRID(Frame& fr);
		void PRIV(Frame& fr);

		void T0_Z(Frame& fr);
		void W0_Z(Frame& fr);

	private:
		FILE *mp3;
		char *fileName;
		Header header;
		vector<Frame> frame;
};
void ID3v2::deal(void) {
	getHeader();
	while(getFrame());

	for(u_int i = 0; i < frame.size(); ++i) {
		//frame[i].print();
	}
}
FrameHeader ID3v2::getFrameHeader(void) {
	char str[10];
	read(str, 10);
	u_int len = 0;
	for(int i = 4; i < 8; ++i)
		len = (len << 8) | (str[i] & 0x000000ff);
	FrameHeader fh(str, len, str+8);
	return fh;
}

bool ID3v2::getFrame(void) {
	FrameHeader fh = getFrameHeader();
	if(!fh.isValid()) return false;
	char str[fh.size];
	read(str, fh.size);
	Frame fm(str, fh);

	dealTag(fm);
	return true;
}
void ID3v2::dealTag(Frame& fr) {
	char str[4];
	strncpy(str, fr.getHeader().id, 4);
	if(strncmp(str, "USLT", 4) == 0) { USLT(fr); }
	else if(strncmp(str, "UFID", 4) == 0) { UFID(fr); }
	else if(strncmp(str, "TXXX", 4) == 0) { TXXX(fr); }
	else if(strncmp(str, "APIC", 4) == 0) { APIC(fr); }
	else if(strncmp(str, "COMM", 4) == 0) { COMM(fr); }
	else if(strncmp(str, "SYLT", 4) == 0) { SYLT(fr); }
	else {
		T0_Z(fr);
	}
	//if(strncmp(str, "USLT", 4) == 0) { USLT(fr); }
}
void ID3v2::getHeader(void) {
	char str[10];
	read(str, 10);
	memcpy(header.id, str, 3);
	memcpy(header.version, str + 3, 2);
	header.flag = str[5];
	header.size = 0;
	for(int i = 6; i < 10; ++i)
		header.size = (header.size << 7) | (str[i] & 0x0000007f);
}

void ID3v2::read(char *str, u_int size) {
	//char ss[32];
	u_int n = fread(str, sizeof(char), size, mp3);
	if(n < size) {
		printf("read error: read less than need.\n");
		exit(0);
	}
}


void parsing(const char* file);
int
main(int argc, char **argv) {
	if(argc < 1) {
		fprintf(stderr, "usage: you must input a mp3 file name.\n");
		exit(-1);
	}
	ID3v2 id3(argv[1]);

	return 0;
}
void ID3v2::UFID(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Owner identifier: %s \n", data);
	u_int i = 0; 
	while(data[i] != 0) ++i;
	printf("Identifier: ");
	for(++i; i < fr.getHeader().size; ++i) {
		putchar(data[i]);
	}
	putchar('\n');
}
void ID3v2::TXXX(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Text encoding: %x \nDescription: ", data[0]);
	u_int i = 4;
	while(data[i] != 0) {
		putchar(data[i]);
		++i;
	}
	printf("\nValue: ");
	for(++i; i < fr.getHeader().size; ++i) {
		putchar(data[i]);
	}
	putchar('\n');

}
void ID3v2::WXXX(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Text encoding: %x \nDescription: ", data[0]);
	u_int i = 4;
	while(data[i] != 0) {
		putchar(data[i]);
		++i;
	}
	printf("\nURL: ");
	for(++i; i < fr.getHeader().size; ++i) {
		putchar(data[i]);
	}
	putchar('\n');

}
void ID3v2::IPLS(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Text Encoding: %x \nPeople list strings: ", data[0]);
	for(u_int i = 1; i < fr.getHeader().size && data[i] != 0; ++i)
		putchar(data[i]);
	putchar('\n');

}
void ID3v2::MCDI(Frame& fr) {
	const char *data = fr.getData();
	printf("CD TOC: ");
	//termnited none;
	for(u_int i = 0; i < fr.getHeader().size; ++i)
		putchar(data[i]);
	putchar('\n');
}
void ID3v2::ETCO(Frame& fr) {
	const char *data = fr.getData();
	printf("Time stamp format: %c \n", data[0]);
	//termnited none;
}
void ID3v2::MLLT(Frame& fr) {}
void ID3v2::SYTC(Frame& fr) {}
void ID3v2::USLT(Frame& fr) {
	const char *data = fr.getData();
	char lang[4] = {0};
	fr.getHeader().print();
	strncpy(lang, data + 1, 3);
	printf("Text encoding: %x \nLanguage: %s \nContent descriptor: ", data[0], lang);
	u_int i = 4;
	while(data[i] != 0) {
		putchar(data[i]);
		++i;
	}
	printf("\nLyrics text: ");
	for(++i; i < fr.getHeader().size; ++i) {
		putchar(data[i]);
	}
	putchar('\n');
}
void ID3v2::SYLT(Frame& fr) {
	const char *data = fr.getData();
	char lang[4] = {0};
	fr.getHeader().print();
	strncpy(lang, data + 1, 3);
	printf("Text encoding: %x \nLanguage: %s \nTime stamp format: %x \nContent type: %x \n", data[0], lang, data[4], data[5]);
	printf("\nContent descriptor: %s \n", data + 6);
}
void ID3v2::COMM(Frame& fr) {
	const char *data = fr.getData();
	char lang[4] = {0};
	fr.getHeader().print();
	strncpy(lang, data + 1, 3);
	printf("Text encoding: %x \nLanguage: %s \nShort content descrip: ", data[0], lang);
	u_int i = 4;
	while(data[i] != 0) {
		putchar(data[i]);
		++i;
	}
	printf("\nThe actual text: ");
	for(++i; i < fr.getHeader().size; ++i) {
		putchar(data[i]);
	}
	putchar('\n');
}
void ID3v2::RVAD(Frame& fr) {}
void ID3v2::EQUA(Frame& fr) {}
void ID3v2::RVRB(Frame& fr) {}
void ID3v2::APIC(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Text encoding: %x \nMIME type: %s \nPicture type: ", data[0], data + 1);
	u_int i = 1;
	while(data[i++] != 0);
	assert( strncmp(data + 1, "image/", 6) == 0);
	const char *mime = data + 7;
	printf("%x \nDescription: ", data[i]);
	while(data[++i] != 0) 
		putchar(data[i]);
	char picFile[strlen(fileName) + 16];
	strcpy(picFile, fileName);
	strcat(picFile, ".");
	FILE *PIC = fopen(strcat(picFile, mime), "wb");
	assert(PIC != NULL);
	for(++i; i < fr.getHeader().size; ++i) {
		fprintf(PIC, "%c", data[i]);
	}
	fclose(PIC);
}
void ID3v2::GEOB(Frame& fr) {}
void ID3v2::PCNT(Frame& fr) {}
void ID3v2::POPM(Frame& fr) {}
void ID3v2::RBUF(Frame& fr) {}
void ID3v2::AENC(Frame& fr) {}
void ID3v2::LINK(Frame& fr) {}
void ID3v2::POSS(Frame& fr) {}
void ID3v2::USER(Frame& fr) {}
void ID3v2::OWNE(Frame& fr) {}
void ID3v2::COMR(Frame& fr) {}
void ID3v2::ENCR(Frame& fr) {}
void ID3v2::GRID(Frame& fr) {}
void ID3v2::PRIV(Frame& fr) {}

void ID3v2::T0_Z(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("Text Encoding: %x \nInformation: ", data[0]);
	for(u_int i = 1; i < fr.getHeader().size && data[i] != 0; ++i)
		putchar(data[i]);
	putchar('\n');
}
void ID3v2::W0_Z(Frame& fr) {
	const char *data = fr.getData();
	fr.getHeader().print();
	printf("URL: ");
	for(u_int i = 0; i < fr.getHeader().size && data[i] != 0; ++i)
		putchar(data[i]);
	putchar('\n');
}

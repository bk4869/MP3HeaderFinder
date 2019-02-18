/* MP3 Header Finder */
/* Only work with MPEG Layer 3 File*/
/* Yumin Chen */

#include <stdio.h>

#define MAX_SIZE_ALLOW 10485760
#define BYTE_TO_MB 1048576



FILE * fp;
float size = 0;
unsigned char * data;
int header;

int main( int argc, char ** argv )
{
	
	// Open the file given on the command line
	int wrongInput = initialize(argc,argv);
	if(wrongInput == 0){
		return 0;
	}
	
	// How many bytes are there in the file? 
	//	Here we use ANSI standard function calls.
	fseek( fp, 0, SEEK_END );		// go to 0 bytes from the end
	size = ftell(fp);				// how far from the beginning?
	rewind(fp);						// go back to the beginning
	
	do{
		if( size < 1 || size > MAX_SIZE_ALLOW )
		{
			printf("File size is not within the allowed range\n"); 
			break;
		}
	
		printf( "File size: %.02f MB\n", size/BYTE_TO_MB);
		
		// Allocate memory on the heap for a copy of the file
		data = (unsigned char *)malloc(size);
		// Read it into our block of memory
		size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
		if( bytesRead != size )
		{
			printf( "Error reading file. Unexpected number of bytes read: %d\n",bytesRead );
			break;
		}

		//Find Header
		findHeader();
		//Is MP3 Layer 3
		if(isL3() != 0){
			printf("It's a MPEG Layer 3 file\n");
		}else{
			printf("It's NOT a MPEG Layer 3 file\n");
		}

		//Call method to show file details
		bitRate();
		findSampRate();
		copyRight();

		break;
		
	}while(0);
		fclose(fp);				// close and free the file
		exit(0);

}	

//Determine is the input valid
int initialize(int argc, char ** argv){

	int bool = 1;

	if( argc != 2 )
	{
		printf( "Usage: %s filename.mp3\n", argv[0] );
		return 0;
	}
	
	fp = fopen(argv[1], "rb");

	if( fp == NULL )
	{
		printf( "Can't open file %s\n", argv[1] );
		return 0;
	}

	return bool;

}

//Find MP3 Header from input source
int findHeader(){
	// Finding the Sync Word
	int i=0;
	while((unsigned)data[i] != 0xFF){
		i++;
	}

	header = i;
	//Finding the header
	int j;
	printf("MP3 header is ");
	for(j=0; j<4; j++){
		
		printf("%x", (unsigned)data[i+j]);
	}
	printf("\n");

	return i;
}

//Find is MPEG Layer
//If the Hex is equal to A(1010) or B(1011) then is not 
int isL3(){
	char l3tag = (unsigned)data[header+1] & 0x0f;

	if(l3tag != 0xa && l3tag != 0xb){
		return 0;
	}
}

//Find bit rate
void bitRate(){
	char bit = (unsigned)data[header+2]  >> 4;
	//bit rate library
	int bitRate[] = {0,0,0,32,40,48,56,64,80,96,112,128,0,160,192,224,254,320};
	//Find the right index
	int j =0;
	for(int i=0x00; i<bit; i++){
		j++;
	}
	
	printf("The Bit Rate is %d kbit/s\n", bitRate[j]);
}

//Finding the Sample Rate
void findSampRate(){
	//Find the right bit location for sample rate info
	char sr = (unsigned)data[header+2]  & 0xf;
	char firstTwoBits = sr >> 2;
	//Sample rate library
	int sampleRate[] = {24000,32000,44100,48000};

	//Find the index on library
	int j=0;
	for(int i=0x00; i<firstTwoBits; i++){
		j++;
	}
	printf("The Sample Rates is %d Hz\n", sampleRate[j]);
}

void copyRight(){
	char cr = (unsigned)data[header+3] & 0xf;
	char firstTwoBits = cr >> 2;
	
	int j=0;
	for(int i=0x00; i<firstTwoBits; i++){
		j++;
	}
	if(j==0 || j==1){
		printf("Not Copyrighted!\n");
		if(j==0){
			printf("Copy of Original Media!\n");
		}else{
			printf("Not a Copy of Original Media!\n");
		}
	}
	if(j==2 || j==3){
		printf("Copyrighted!\n");
		if(j==0){
			printf("Copy of Original Media!\n");
		}else{
			printf("Not a Copy of Original Media!\n");
		}
	}

	
}






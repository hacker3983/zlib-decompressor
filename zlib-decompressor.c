#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <stdbool.h>
#include <errno.h>

void decompress_file(const char* program_name, const char* filepath) {
	FILE* f = fopen(filepath, "rb");
	if(!f) {
		int errcode = errno;
		fprintf(stderr, "%s: error: It appears the specified file %s does not exists",
				program_name, filepath);
		exit(errcode);
	}
	fseek(f, 0, SEEK_END);
	uLongf compressed_datalen = ftell(f);
	rewind(f);
	Bytef *compressed_data = malloc(compressed_datalen * sizeof(Bytef)),
	      *uncompressed_data = malloc(sizeof(Bytef));
	uLongf uncompressed_datalen = 1;
	if(fread(compressed_data, sizeof(Bytef), compressed_datalen, f) != compressed_datalen) {
		int errcode = errno;
		fprintf(stderr, "%s: error: Failed to read compressed data from file %s\n", program_name, filepath);
		exit(errcode);
	}
	//printf("Successfully read compressed data from file %s\n", filepath);
	int uncompression_status = 0, chunk_size = 65535;
	while((uncompression_status = uncompress(uncompressed_data, &uncompressed_datalen,
				compressed_data, compressed_datalen)) != Z_OK)
	{
		if(uncompression_status == Z_BUF_ERROR) {
			uncompressed_datalen += chunk_size;
			free(uncompressed_data); uncompressed_data = NULL;
			uncompressed_data = malloc(uncompressed_datalen * sizeof(Bytef));
		} else {
			fprintf(stderr, "Error: Failed to decompress data\n");
			exit(uncompression_status);
			printf("Error Types: Z_MEM_ERROR: %d, Z_DATA_ERROR: %d, Z_BUF_ERROR: %d\n", Z_MEM_ERROR,
					Z_DATA_ERROR, Z_BUF_ERROR);
			printf("uncompression status code: %d\n", uncompression_status);
		}
	}
	//printf("Allocated memory of size %zu bytes for uncompressed data\n",
	//			uncompressed_datalen * sizeof(Bytef));
	printf("Successfully uncompressed data from file %s\n", filepath);
	fwrite(uncompressed_data, sizeof(Bytef), uncompressed_datalen, stdout);
	printf("\n");
	free(compressed_data); compressed_data = NULL;
	free(uncompressed_data); uncompressed_data = NULL;
	fclose(f);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s [FILENAMES...]\n"
				"Please specify a filename as an argument example: file1 file2 file3\n", argv[0]);
		exit(ENOENT);
	}
	for(int i=1;i<argc;i++) {
		printf("[ \033[31m*\033[0m ] Decompressing file %s...\n", argv[i]);
		decompress_file(argv[0], argv[i]);
	}
	return 0;
}

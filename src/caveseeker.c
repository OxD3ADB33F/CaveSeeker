#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define COLOR "\x1b[32m"
#define RESET "\x1b[0m"

typedef struct {
    long offset;
    size_t size;
    long start_address;
    long end_address;
} code_cave_t;

void
finder (const char *filename, size_t min_size) 
{
  FILE *file = fopen(filename, "rb");
  if (!file) 
    {
      perror("[!] File not open");
      exit(EXIT_FAILURE);
    }

  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t *buffer = (uint8_t *)malloc(filesize);
  if (!buffer) 
    {
      perror("[!] Error allocating memory");
      fclose(file);
      exit(EXIT_FAILURE);
    }

  fread(buffer, 1, filesize, file);
  fclose(file);

  code_cave_t *caves = (code_cave_t *)malloc(filesize * sizeof(code_cave_t));
  size_t cave_count = 0;

  for (long i = 0; i < filesize;) 
    {
      if (buffer[i] == 0x00) 
        {
          long start = i;
          while (i < filesize && buffer[i] == 0x00) 
            {
              i++;
            }
          long end = i;
          size_t size = end - start;
          if (size >= min_size) 
            {
              caves[cave_count].offset = start;
              caves[cave_count].size = size;
              caves[cave_count].start_address = start;
              caves[cave_count].end_address = end;
              cave_count++;
            }
        } 
      else 
        {
          i++;
        }
    }

  if (cave_count > 0) 
    {
      printf(COLOR "[+] " RESET "%zu code caves were found:\n", cave_count);
      for (size_t i = 0; i < cave_count; i++) 
        {
          printf(COLOR "[+] " RESET "Code cave #%zu:\n", i + 1);
          printf("[*] Cave Size: %zu bytes\n", caves[i].size);
          printf("[*] Start Address: 0x%lx\n", caves[i].start_address);
          printf("[*] End Address: 0x%lx\n", caves[i].end_address);
          printf("[*] File Offset: 0x%lx\n", caves[i].offset);
          printf("\n");
        }
    } 
  else 
    {
      printf("[!] No code cave found\n");
    }

  free(buffer);
  free(caves);
}

int 
main(int argc, char *argv[]) 
{
  if (argc != 3) 
    {
      fprintf(stderr, "Use: %s <ELF> <min_code_cave_size>\n", argv[0]);
      return EXIT_FAILURE;
    }

  size_t min_size = strtoul(argv[2], NULL, 10);
  finder(argv[1], min_size);

  return EXIT_SUCCESS;
}

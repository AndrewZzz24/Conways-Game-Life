#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

struct Bmp {

    int w;
    int h;
    int size;

} image;

void GameLife(int **life, int height, int width) {

    int alive;
    int **temp = (int **) malloc(sizeof(int *) * height);

    for (int i = 0; i < height; ++i)
        temp[i] = (int *) malloc(sizeof(int) * width);

    int x, y;
    int inf = 0;

    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            temp[y][x] = life[y][x];

    for (y = 1; y < height - 1; y++) {

        for (x = 1; x < width - 1; x++) {

            alive = life[y + 1][x - 1] + life[y + 1][x] + life[y + 1][x + 1]
                    + life[y][x - 1] + life[y][x + 1] + life[y - 1][x - 1]
                    + life[y - 1][x] + life[y - 1][x + 1];

            if (temp[y][x] == 1) {

                if ((alive == 2) || (alive == 3))
                    temp[y][x] = life[y][x];

                if ((alive > 3) || (alive < 2))
                    temp[y][x] = 0;

            } else if (alive == 3)
                temp[y][x] = 1;

        }

    }

    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++) {

            if (life[y][x] == temp[y][x])
                inf++;

            life[y][x] = temp[y][x];

        }

    for (int i = 0; i < height; ++i)
        free(temp[i]);

    free(temp);

    if (inf == height * width)
        exit(0);

}

void gameset(FILE *file, int maxiter, int dumpfreq, char *dirname) {

    unsigned char header[54];
    int i, j, k, l, m;

    fread(header, sizeof(unsigned char), 54, file);
    image.w = header[21] * 256 * 256 * 256 + header[20] * 256 * 256 +
              header[19] * 256 + header[18];

    image.h = header[25] * 256 * 256 * 256 + header[24] * 256 * 256 +
              header[23] * 256 + header[22];

    image.size = header[5] * 256 * 256 * 256 + header[4] * 256 * 256 +
                 header[3] * 256 + header[2];

    unsigned char *imagebyte = (unsigned char *) malloc((image.size - 54) *
                                                        sizeof(unsigned char));

    fread(imagebyte, sizeof(unsigned char), image.size, file);
    int **img = (int **) malloc(image.h * sizeof(int *));

    for (i = 0; i < image.h; i++)
        img[i] = (int *) malloc(image.w * sizeof(int));

    k = 0;

    for (i = image.h - 1; i >= 0; i--) {

        for (j = 0; j < image.w; j++) {

            if (imagebyte[k] == 255)
                img[i][j] = 0;

            else
                img[i][j] = 1;
            k += 3;

        }

    }
    char way1[256];
    char filename1[16];

    strcpy(way1, dirname);
    strcat(way1, "\\");
    strcat(way1, _itoa(0, filename1, 10));
    strcat(way1, ".bmp");

    FILE *f1 = fopen(way1, "wb");
    unsigned char *imagebyte1 = imagebyte;
    fwrite(header, 1, 54, f1);

    m = 0;

    for (i = image.h - 1; i >= 0; i--) {

        for (j = 0; j < image.w; j++) {

            for (k = 0; k < 3; k++) {

                if (img[i][j] == 1)
                    imagebyte1[m] = 0;
                else
                    imagebyte1[m] = 255;
                m++;
            }

        }

    }

    fwrite(imagebyte1, sizeof(unsigned char), image.size, f1);
    fclose(f1);

    for (l = 1; l <= maxiter; l++) {

        GameLife(img, image.h, image.w);

        if (l % dumpfreq != 0)
            continue;

        char way[256];
        char filename[16];

        strcpy(way, dirname);
        strcat(way, "\\");
        strcat(way, _itoa(l, filename, 10));
        strcat(way, ".bmp");

        FILE *life = fopen(way, "wb");

        fwrite(header, 1, 54, life);

        m = 0;

        for (i = image.h - 1; i >= 0; i--) {

            for (j = 0; j < image.w; j++) {

                for (k = 0; k < 3; k++) {

                    if (img[i][j] == 1)
                        imagebyte[m] = 0;
                    else
                        imagebyte[m] = 255;
                    m++;
                }

            }

        }

        fwrite(imagebyte, sizeof(unsigned char), image.size, life);
        fclose(life);

    }
    for (i = 0; i < image.h; i++)
        free(img[i]);

    free(img);

}

int main(int argc, char *argv[]) {

    int maxiter = 1000, dumpfreq = 1;
    char *dirname;
    FILE *file;

    for (int i = 0; i < argc; i++) {

        if (!strcmp("--input", argv[i]))
            file = fopen(argv[1 + 1], "rb");

        if (!strcmp("--output", argv[i])) {

            dirname = argv[i + 1];
            _mkdir(dirname);

        }

        if (!strcmp("--max_iter", argv[i]))
            maxiter = strtol(argv[i + 1], 0, 10);

        if (!strcmp("--dump_freq", argv[i]))
            dumpfreq = strtol(argv[i + 1], 0, 10);

    }

    gameset(file, maxiter, dumpfreq, dirname);

    return 0;

}

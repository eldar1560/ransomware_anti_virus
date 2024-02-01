//#include <stdio.h>
//#include <stdlib.h>
//#include <limits.h>
//#include <math.h>
//
//static void fill_histogram(size_t histogram[], FILE* file)
//{
//    int c;
//    while ((c = getc(file)) != EOF) {
//        /* c must be positive and in range */
//        ++histogram[c];
//    }
//}
//
///* Histogram array length must be UCHAR_MAX+1 */
//static size_t total(const size_t histogram[])
//{
//    size_t sum = 0;
//    for (size_t i = 0; i <= UCHAR_MAX; ++i) {
//        sum += histogram[i];
//    }
//    return sum;
//}
//
///* Histogram array length must be UCHAR_MAX+1 */
//static double entropy(const size_t histogram[])
//{
//    size_t len = total(histogram);
//
//    double result = 0.0;
//    for (size_t i = 0; i <= UCHAR_MAX; ++i) {
//        if (!histogram[i]) {
//            continue;
//        }
//        double freq = (double)histogram[i] / (double)len;
//        result -= freq * log10(freq) / log10(2.0);
//    }
//
//    return result;
//}
//
//
//int main(int argc, char const** argv)
//{
//    FILE* f = stdin;
//    if (argc > 2) {
//        fprintf(stderr, "too many arguments\n");
//        return EXIT_FAILURE;
//    }
//    else if (argc == 1) {
//        f = fopen(argv[1], "rb");
//        if (!f) {
//            perror(argv[1]);
//            return EXIT_FAILURE;
//        }
//    }
//
//    size_t histogram[UCHAR_MAX + 1] = { 0 };
//    fill_histogram(histogram, f);
//
//    if (f != stdin) {
//        if (fclose(f)) {
//            perror("close");
//            return EXIT_FAILURE;
//        }
//    }
//
//    printf("Length of file: %zu\n", total(histogram));
//    printf("Entropy of file: %f\n", entropy(histogram));
//    return EXIT_SUCCESS;
//}
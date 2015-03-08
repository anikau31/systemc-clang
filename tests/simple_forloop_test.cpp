int sc_main(int argc, char *argv[]) {

    int a[5] = { 0 };
    int b[10] = { 1 };
    int c[15] = { 2 };
    int d[20] = { 3 };

    for (int i = 0; i < 5; ++i) {

        a[i] = b[i * 2];

        a[i] = b[i * 2] + a[i] * 5;

        a[i] = c[i * 3] % (b[i * 2] + i);

        d[i * 4] = a[i] % (c[i * 3] + i);
    }
}

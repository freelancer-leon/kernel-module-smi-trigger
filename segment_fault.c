// gcc -Wall -g -O0 -o segment_fault segment_fault.c

unsigned long foo = 0xdeadbeef;

int main() {
    long i, j = 0;

    for (i = 0; i < 4248000000; i++) {
       j++;
    }

    void (*func)() = (void (*)())&foo;
    func();
    return 0;
}

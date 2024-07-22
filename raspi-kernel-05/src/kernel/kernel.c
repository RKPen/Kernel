#include <stddef.h>
#include <stdint.h>
#include <kernel/uart.h>
#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdio.h>
#include <common/stdlib.h>

// Utility functions
int custom_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

int custom_atoi(const char *str) {
    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str) {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        }
        str++;
    }
    return sign * result;
}

void printf(const char *format, ...) {
    char *arg_list = (char *)&format + sizeof(format);
    char *str_arg;
    int int_arg;
    
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            format++;
            switch (*format) {
                case 's':
                    str_arg = *(char **)arg_list;
                    puts(str_arg);
                    arg_list += sizeof(char *);
                    break;
                case 'd':
                    int_arg = *(int *)arg_list;
                    puts(itoa(int_arg));
                    arg_list += sizeof(int);
                    break;
                default:
                    putc('%');
                    putc(*format);
                    break;
            }
        } else {
            putc(*format);
        }
        format++;
    }
}

// Caesar cipher functions
void encrypt(char *text, int shift) {
    while (*text) {
        if (*text >= 'a' && *text <= 'z') {
            *text = (*text - 'a' + shift) % 26 + 'a';
        } else if (*text >= 'A' && *text <= 'Z') {
            *text = (*text - 'A' + shift) % 26 + 'A';
        }
        text++;
    }
}

void decrypt(char *text, int shift) {
    encrypt(text, 26 - shift);  // Decrypting by reversing the shift
}

// LinkedList functions
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *head = NULL;

Node* create_node(int data) {
    Node *new_node = (Node *)kmalloc(sizeof(Node));
    if (new_node) {
        new_node->data = data;
        new_node->next = NULL;
    }
    return new_node;
}

void add_node(int data) {
    Node *new_node = create_node(data);
    if (!head) {
        head = new_node;
    } else {
        Node *temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

void display_list() {
    Node *temp = head;
    while (temp) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    puts("NULL\n");
}

void clear_list() {
    Node *temp;
    while (head) {
        temp = head;
        head = head->next;
        kfree(temp);
    }
}

// Main kernel function
void kernel_main(atag_t *atags) {
    uart_init();
    mem_init(atags);  // Pass the atags argument to mem_init
    puts("CMPS240 Project Summer 2024!\n");

    char buf[256];
    while (1) {
        puts(">");
        gets(buf, 256);

        if (custom_strcmp(buf, "help") == 0) {
            puts("Available commands: help, sum, encrypt, decrypt, addnode, displaylist, clearlist, exit\n");
        } else if (custom_strcmp(buf, "sum") == 0) {
            puts("Enter two integers:\n");
            gets(buf, 256);
            int a = custom_atoi(buf);
            gets(buf, 256);
            int b = custom_atoi(buf);
            printf("Sum: %d\n", a + b);
        } else if (custom_strcmp(buf, "encrypt") == 0) {
            puts("Enter shift value:\n");
            gets(buf, 256);
            int shift = custom_atoi(buf);
            puts("Enter text to encrypt:\n");
            gets(buf, 256);
            encrypt(buf, shift);
            printf("Encrypted: %s\n", buf);
        } else if (custom_strcmp(buf, "decrypt") == 0) {
            puts("Enter shift value:\n");
            gets(buf, 256);
            int shift = custom_atoi(buf);
            puts("Enter text to decrypt:\n");
            gets(buf, 256);
            decrypt(buf, shift);
            printf("Decrypted: %s\n", buf);
        } else if (custom_strcmp(buf, "addnode") == 0) {
            puts("Enter an integer to add to the LinkedList:\n");
            gets(buf, 256);
            int value = custom_atoi(buf);
            add_node(value);
        } else if (custom_strcmp(buf, "displaylist") == 0) {
            display_list();
        } else if (custom_strcmp(buf, "clearlist") == 0) {
            clear_list();
        } else if (custom_strcmp(buf, "exit") == 0) {
            break;
        } else {
            puts("Unknown command. Type 'help' for a list of commands.\n");
        }

        // Clear the buffer to prevent residual data
        for (int i = 0; i < 256; i++) {
            buf[i] = '\0';
        }
    }
}

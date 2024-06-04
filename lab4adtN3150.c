#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


void get_v_option_information() {
    printf("Травкин Александр Дмитриевич, гр. N3150\nВариант: 1-7-19-8\n");
    exit(0);
}


FILE *data_file;
char *data_file_name;
unsigned int *shifts;
unsigned short *indexes;
int count_of_elements = 0;

struct element {
    char *data;
    struct element *next;
};

struct element *head = NULL;
struct element *tail = NULL;


struct element *get(int index) {
    struct element *item = head;
    for (int i = 0; i < index; i++) {
        item = item->next;
    }
    return item;
}


void set(char *string, int index) {
    struct element *item = head;
    for (int i = 0; i < index; i++) {
        item = item->next;
    }
    item->data = malloc(sizeof(string));
    strcpy(item->data, string);
}

// функция создаёт пустой список заданной длинны
void create_empty_list() {
    struct element example;
    head = malloc(sizeof(example));
    struct element *item = head;
    for (int i = 1; i < count_of_elements; i++) {
        item->next = malloc(sizeof(example));
        item = item->next;
        item->next = NULL;
    }
    tail = item;
}

// функция считывает из файла отступы и индексы
void get_shifts_and_indexes() {
    shifts = calloc(10, sizeof(int));
    bool read = true;
    unsigned int shift_index = 0;
    
    while (read)
    {
        char *data = calloc(4, sizeof(char));
        if (shift_index % 10 == 0) {
            shifts = realloc(shifts, (shift_index + 10) * sizeof(int));
        }

	// Считываем 4 байта отступа
        for (int i = 0; i < 4; i++) {
            data[i] = fgetc(data_file);
        }

	// При нулевом отступе заканчиваем считывать
        if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0) {
            read = false;
        } else {
            shifts[shift_index] = *((unsigned int*)data);
        }
        
        shift_index++;
        fflush(stdin);
    }
    
    // Количество элементов = количеству отступов
    count_of_elements = shift_index - 1;
    indexes = calloc(count_of_elements, sizeof(short));

    fseek(data_file, -2*count_of_elements, SEEK_END);

    // Считывание индексов
    for (int i = 0; i < count_of_elements; i++) {
        char data[2];
        data[0] = fgetc(data_file);
        data[1] = fgetc(data_file);
        indexes[i] = *((unsigned short*)data);
        fflush(stdin);
    }
}


void push_back(char *string) {
    struct element *new_element = malloc(sizeof(struct element));
    new_element->data = string;
    new_element->next = NULL;
    if (count_of_elements == 0) {
        head = new_element;
        tail = new_element;
    } else {
        tail->next = new_element;
        tail = new_element;
    }
    count_of_elements++;
}


void push_front(char *string) {
    struct element example;
    struct element *new_element = malloc(sizeof(example));
    new_element->data = string;
    if (count_of_elements == 0) {
        head = new_element;
        tail = new_element;
    } else {
        new_element->next = head;
        head = new_element;
    }
    count_of_elements++;
}


void pop_back() {
    if (count_of_elements >= 2) {
        struct element *for_delete = tail;
        tail = get(count_of_elements - 2);
        tail->next = NULL;
        free(for_delete);
        count_of_elements--;
    } else if (count_of_elements == 1) {
        head = NULL;
        tail = NULL;
        count_of_elements--;
    }
}


void pop_front() {
    if (count_of_elements >= 2) {
        struct element *for_delete = head;
        head = get(1);
        free(for_delete);
        count_of_elements--;
    } else if (count_of_elements == 1) {
        head = NULL;
        tail = NULL;
        count_of_elements--;
    }
}


void dump() {
    for (int i = 0; i < count_of_elements; i++) {
        struct element *to_dump = get(i);
        printf("%p %p %s\n", to_dump, to_dump->next, to_dump->data);
    }
}


void dump_to_file(char *file_name) {
    FILE *output_file;
    output_file = fopen(file_name, "w");
    for (int i = 0; i < count_of_elements; i++) {
        struct element *to_dump = get(i);
        char string_to_write[5000];
        // Функция snprintf склеивает строки по регулярному выражению
        snprintf(string_to_write, sizeof(string_to_write), "%p %p %s\n", to_dump, to_dump->next, to_dump->data);
        fputs(string_to_write, output_file);
    }
}

// функция считывает строки из файла и записывает их в список
void load_strings_to_list() {
    char string[5000];
    for (int string_index = 0; string_index < count_of_elements; string_index++) {
        fseek(data_file, shifts[string_index], SEEK_SET);
        fgets(string, 5000, data_file);
        set(string, indexes[string_index]);
    }
}

// проверка содержимого строки на целое число
bool is_it_uint(char *str) {
    for (int i = 0; i < (int)strlen(str); i++) {
        if (!('0' <= str[i] && str[i] <= '9')) {
            return false;
        }
    }
    return true;
}


// Функция преобразовывающая строку в unsigned int
unsigned int get_uint_from_string(char *str[]) {
    int power_of_ten = 0;
    int index = strlen(*str);
    unsigned int value = 0;
    // Пройдём по всем элементам полученной строки и переведём их в соответствующие им числовые значения
    while (index) {
        // Прибавляем к value значение каждого символа домноженое на его степень 10
        value += (pow(10, power_of_ten)) * (int)((*str)[index - 1] - '0'); 
        power_of_ten += 1;
        index -= 1;
    }
    return value;
}

// функция вставки строки на определённую позицию
void insert(int index, char *string) {
    if (0 <= index && index < count_of_elements) {
        struct element *new_element = malloc(sizeof(struct element));
        new_element->data = malloc(sizeof(string));
        strcpy(new_element->data, string);

        struct element *previous_element = head;
        for (int i = 1; i < index; i++) {
            previous_element = previous_element->next;
        }

        struct element *next_element = previous_element->next;
        previous_element->next = new_element;
        new_element->next = next_element;
        count_of_elements++;
    }
}


void write_int_to_data_file(unsigned int number) {
    // Записываем каждый байт числа в файл
    for (long unsigned int i = 0; i < sizeof(int); i++) {
        unsigned char byte = (number >> (8 * i)) & 0xFF; // Получаем i-й байт числа
        fwrite(&byte, sizeof(unsigned char), 1, data_file); // Записываем байт в файл
    }
}


void write_short_to_data_file(unsigned short number) {
    // Записываем каждый байт числа в файл
    for (long unsigned int i = 0; i < sizeof(unsigned short); i++) {
        unsigned char byte = (number >> (8 * i)) & 0xFF; // Получаем i-й байт числа
        fwrite(&byte, sizeof(unsigned char), 1, data_file); // Записываем байт в файл
    }
}

// функция сохраняет данные из списка в файл
void save_data_file() {
    unsigned int shift = (count_of_elements + 1) * 4;

    shifts = calloc(count_of_elements + 1, sizeof(unsigned int));
    indexes = calloc(count_of_elements, sizeof(unsigned short));

    for (int i = 0; i < count_of_elements; i++) {
        shifts[i] = shift;
        indexes[i] = i;
        shift += strlen(get(i)->data) + 1;
    }
    shifts[count_of_elements + 1] = 0;
    
    //Очистка файла
    fclose(data_file);
    data_file = fopen(data_file_name, "w");
    fclose(data_file);
    data_file = fopen(data_file_name, "wb");

    for (int i = 0; i < count_of_elements; i++) {
        write_int_to_data_file(shifts[i]);
    }
    if (count_of_elements != 0) {
        write_int_to_data_file(0);
    }

    struct element *item = head;
    for (int i = 0; i < count_of_elements; i++) {
        fputs(item->data, data_file);
        fputc(0, data_file);
        item = item->next;
    }

    for (unsigned short i = 0; i < count_of_elements; i++) {
        indexes[i] = i;
        write_short_to_data_file(i);
    }
}

// функция, которая возвращает массив строк в одинарных кавыках, вырезанных из строки комманды
char** extract_strings(const char* input, int* count) {
    int num_quotes = 0;
    const char* p = input;

    // Подсчитываем количество одинарных кавычек
    while (*p) {
        if (*p == '\'') {
            num_quotes++;
        } else if (*p == '\\' && *(p + 1) == '\'') {
            p++; // Пропускаем экранированные кавычки
        }
        p++;
    }

    // Выделяем память для массива подстрок
    char** substrings = (char**)malloc(num_quotes * sizeof(char*));
    if (!substrings) {
        *count = 0;
        return NULL;
    }

    int len = p - input;
    char *string = malloc(len);
    p = input;
    int i = 0;
    bool read = false;
    while (p - input < len) {
        if (*p == '\'') {
            if (*(p - 1) != '\\') {
                if (read) {
                    string[i] = *p;
                    string[i + 1] = 0;
                    substrings[*count] = malloc(i + 2);
                    strcpy(substrings[*count], string);
                    *count += 1;
                    read = false;
                    i = 0;
                } else {
                    string[i] = *p;
                    read = true;
                    i++;
                }
            } else if (read) {
                string[i] = *p;
                i++;
            }
        } else if (read) {
            string[i] = *p;
            i++;
        }
        p++;
    }

    free(string);
    return substrings;
}


int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Ошибка: не задано имя файла.");
        exit(1);
    } else if (argc > 1) {
        if (strstr(argv[1], "-v")) {
            get_v_option_information();
        }
        if (strstr(argv[1], ".txt") != NULL || strstr(argv[1], ".bin") != NULL || strstr(argv[1], ".bean") != NULL) {
            data_file_name = argv[1];
            data_file = fopen(data_file_name, "r");
            if (!data_file) {
                data_file = fopen(data_file_name, "w");
                fclose(data_file);
                data_file = fopen(data_file_name, "r");
            }
            if (!data_file) {
                printf("Ошибка: неудалось создать файл");
            }
        } else {
            printf("Ошибка: некорректный формат файла.");
            exit(3);
        }
    }

    // Считываем данные из файла

    // Проверяем не пустой ли файл
    fseek(data_file, 0, SEEK_END); 
    long file_size = ftell(data_file);
    fseek(data_file, 0, SEEK_SET);

    if (file_size != 0) {
        // если файл не пустой, считываем данные из фала
        get_shifts_and_indexes();
        create_empty_list();
        load_strings_to_list();
    }

    // Массив строк под комманды
    char **commands;
    commands = calloc(10, sizeof(char*));
    int comand_number = 0;

    // Пока не закончатся строки в файле или пользователь не введёт Ctrl+D считываеи строки
    while (true)
    {
        // Если не хватило места, добавляем памяти
        if (comand_number % 10 == 0 && comand_number > 0) {
            commands = realloc(commands, comand_number * sizeof(char*) + 10);
        }
        // Когда пользователь введёт Ctrl+D fgets вернёт NULL
        if(fgets((commands[comand_number] = calloc(1000, sizeof(char*))), 1000, stdin) == NULL) {
            break;
        }
        comand_number++;
        // Очистка буфера
        fflush(stdin);
    }

    // Обрабатываем команды
    for (int i = 0; i < comand_number; i++) {
        char *command = malloc(strlen(commands[i]));
        strcpy(command, commands[i]);
        if (strstr(command, "push_back") != NULL) {
            int count_of_args = 0;
            // Получаем строки из комманды
            char **strings = extract_strings(command, &count_of_args);
            for (int i = 0; i < count_of_args; i++) {
                push_back(strings[i]);
            }
        } else if (strstr(command, "push_front")) {
            int count_of_args = 0;
            char **strings = extract_strings(command, &count_of_args);
            for (int i = 0; i < count_of_args; i++) {
                if (strcmp("\' \'", strings[i]) != 0) {
                    push_front(strings[i]);
                }
            }
        } else if (strstr(command, "pop_back")) {
            pop_back();
        } else if (strstr(command, "pop_front")) {
            pop_front();
        } else if (strstr(command, "insert")) {
            char *argument;
            // Получаем первый токен
            argument = strtok(command, " ");
            argument = strtok(NULL, " ");
            // Проходим по всем последующим токенам

            int index = 0;

            // Пробуем считать из 2-го слова строки, число
            int count_of_args = 0;
            if (argument != NULL) {
                count_of_args++;
                if (count_of_args == 1) {
                    if (is_it_uint(argument)) {
                        index = get_uint_from_string(&argument);
                    } else {
                        printf("Ошибка: %s не является положительным числом.", argument);
                        exit(5);
                    }
                }
            } else {
                printf("Ошибка: отсуцтвуют аргументы для комманды insert");
            }

            int c = 0;
            char **strings = extract_strings(commands[i], &c);
            if (c >= 1) {count_of_args++;}
            if (count_of_args == 2) {
                insert(index, strings[0]);
            }
        } else if (strstr(command, "dump")) {
            char *file_name = strtok(command, " ");
            int count_of_words = 0;
            while (file_name != NULL) {
                count_of_words++;
                if (count_of_words == 2) {
                    break;
                }
                file_name = strtok(NULL, " ");
            }
            if (file_name != NULL) {
                dump_to_file(file_name);
            } else {
                dump();
            }
        } else {
            printf("Ошибка: неизвестная комманда.");
            break;
        }
        free(command);
        save_data_file();
    }

    // Очистка памяти

    fclose(data_file);

    struct element *p = head;
    struct element *for_free;
    for (; p && p->next != NULL; p = p->next) {
        for_free = p;
        free(p->data);
        p = for_free->next;
        free(for_free);
    }
    free(p);
    free(commands);
}

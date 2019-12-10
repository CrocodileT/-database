#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
FILE * Input;
FILE * Output;
FILE * Detail; //Детальный протокол

///_______________Необходимое_Для_Работы_с_Данными_______________///
typedef struct date {
    int day, month, year;
} date; //выделяется память
typedef struct time {
    int hour, minute;
} time;

enum Weather { error = 0, fair = 1, rain, cloudy, snow };
char WeatherName[][7] = { "error","fair", "rain", "cloudy", "snow" }, WeatherName2[][9] = { "'error'","'fair'", "'rain'", "'cloudy'", "'snow'" };

typedef struct Statistic {
    int s_malloc, s_realloc, s_free;
} Statistic;
Statistic stat = { 0,0,0,0 }; //Переменная для статистики

typedef struct Values { //Список полей
    char fieldname[20]; //Имя поля для обновления
    char value[255]; //Значение
} Values;

typedef struct Data {//Основные данные
    int geo_id, level;     // Идентификатор станции, высота на уровнем моря (м)
    char geo_pos[255];     // Размещение станции, адрес, координаты
    date mea_date;         // Дата наблюдений
    time sunrise, sundown; // Время восхода,захода солнца
    enum Weather weather;       // Общее состояние погоды во время восхода
} Data;

typedef struct Condition { //Описание условия
    char FieldName[10], Sign[10], Constant[100]; //Имя поля для проверки,условие проверки, константа для проверки
    unsigned char FieldIndex; //Номер поля
}  Condition;

bool Error; //Ошибка сравнения
char * ErrorString; //Пояснения к ошибке

bool Check_time(time t) {
    if (t.hour > 24 || t.hour < 0) {
        return(false);
    }
    if (t.minute  > 60 || t.minute < 0) {
        return (false);
    }
    return(true);
}

bool Check_date(date d) {
    if (d.day > 31 || d.day < 0) {
        return(false);
    }
    if (d.month > 12 || d.month < 0) {
        return (false);
    }
    if (d.year < 0) {
        return(false);
    }
    return(true);
}

enum Weather set_weather_by_text(char * value) //Установить enum по его текстовому представлению
{
    for (enum Weather w = fair; w <= snow;)
    {
        if (strcmp(WeatherName[w], value) == 0) return w;
        w = (enum Weather)((int)w + 1);
    }
    for (enum Weather w = fair; w <= snow;)
    {
        if (strcmp(WeatherName2[w], value) == 0) return w;
        w = (enum Weather)((int)w + 1);
    }
    ErrorString = (char*)"Неверная погода";
    Error = true;
    return error;
}

bool ch_geo_id = false;
bool ch_level = false;     // Идентификатор станции, высота на уровнем моря (м)
bool ch_geo_pos = false;     // Размещение станции, адрес, координаты
bool ch_mea_date = false;         // Дата наблюдений
bool ch_sunrise = false;
bool ch_sundown = false; // Время восхода,захода солнца
bool ch_weather = false;


time set_time_by_text(char * value) //Установить время по его текстовому представлению
{
    time result = { 0,0 };
    if (value[1] == ':') {//h:mm
        result.hour = atoi(value); //atoi преобразует символьную строку в число
        result.minute = atoi(&value[2]);
    }
    else
    if (value[2] == ':') {//hh:mm
        result.hour = atoi(value);
        result.minute = atoi(&value[3]);
    }
    else {//Это не время
        Error = true;
        ErrorString = (char*)"Неверное время";
    }
    return result;
}
date set_date_by_text(char * value) //Установить дату по ее текстовому представлению
{
    date result = { 0,0,0 };
    //dd.mm.yyyy, при этом допустимо и d.m.yy
    //Положение первой точки - разделителя
    unsigned int p1 = 0;
    while (p1 < strlen(value) && value[p1] != '.') p1++; //Позиция первой точки
    unsigned int p2 = p1 + 1;
    while (p2 < strlen(value) && value[p2] != '.') p2++; //Позиция второй точки
    if (p2 >= strlen(value))     //если позиция второй точки привышает длину даты, то дата неверна
    {
        Error = true;
        ErrorString = (char*)"Неверная дата";
        return result;
    }
    //Получить значение
    result.day = atoi(value);
    result.month = atoi(&value[p1 + 1]);
    result.year = atoi(&value[p2 + 1]);
    return result;
}

int set_integer_by_text(char * value) //Установить целое число по его текстовому представлению
{
    for (unsigned int k = 0; k < strlen(value); k++)
        if (value[k] != '-' && value[k] != '+' && (value[k]>'9' || value[k] < '0'))
        {
            Error = true;
            ErrorString = (char*)"Неверное целое число";
            return 0;
        }
    return atoi(value);
}

char FieldName[][10] = { "geo_id" , "geo_pos" , "mea_date" , "level" , "sunrise", "weather",  "sundown", "" };

int Index_of_Field(char * fieldname) //Номер поля по его названию
{
    for (int k = 0; FieldName[k][0] != 0; k++)
        if (strcmp(FieldName[k], fieldname) == 0)
            return k;
    return -1; //Нет такого поля
}

bool Compare_integer(int a, int b, char sign[3]) { //Сравнить два целых значения. Вернуть ошибку если они не числа
    if (strcmp(sign, "<") == 0) { return a < b; }
    if (strcmp(sign, ">") == 0) { return a > b; }
    if (strcmp(sign, "==") == 0) { return a == b; }
    if (strcmp(sign, "!=") == 0) { return a != b; }
    if (strcmp(sign, "<=") == 0) { return a <= b; }
    if (strcmp(sign, ">=") == 0) { return a >= b; }
    Error = true;
    ErrorString = (char*) "Неизвестная операция сравнения";
    return false;
}
bool Compare_String(char* a, char* b, char sign[3]) { //Сравнить две строки
    int result = strcmp(a, b);
    return Compare_integer(result, 0, sign);
}

bool Is_Less_date(date a, date b) { //дата меньше?
    if (a.year < b.year) { /*printf("1.1 %d,%d,%d < %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true; }
    if (a.year > b.year) { return false; }
    if (a.month < b.month) { /*printf("1.2 %d,%d,%d < %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true; }
    if (a.month > b.month) { return false; }
    if (a.day <= b.day) { /*printf("1.3 %d,%d,%d < %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true; }
    if (a.day > b.day) { return false; }
    /*printf("1.4 %d,%d,%d > %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/
    return false;
}
bool Is_Greater_date(date a, date b) { //Дата больше?
    if (a.year > b.year) {
        /*printf("2.1 %d,%d,%d > %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true;
    }
    if (a.year < b.year) {
        return false;
    }
    if (a.month > b.month) { /*printf("2.2 %d,%d,%d > %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true; }
    if (a.month < b.month) {
        return false;
    }
    if (a.day >= b.day) { /*printf("2.3 %d,%d,%d > %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);*/ return true; }
    if (a.day < b.day) {
        return false;
    }
    //printf("2.4 %d,%d,%d < %d,%d,%d \n", a.year, a.month, a.day, b.year, b.month, b.day);
    return false;
}
bool Is_Equal_date(date a, date b) { //Даты равны
    return (a.day == b.day && a.month == b.month && a.year == b.year);
}
bool Compare_Date(date a, date b, char sign[3]) //Сравнить две даты
{
    if (strcmp(sign, "<") == 0) { return Is_Less_date(a, b); }
    if (strcmp(sign, ">") == 0) { return Is_Greater_date(a, b); }
    if (strcmp(sign, "==") == 0) { return Is_Equal_date(a, b); }
    if (strcmp(sign, "!=") == 0) { return !Is_Equal_date(a, b); }
    if (strcmp(sign, "<=") == 0) { return Is_Less_date(a, b) || Is_Equal_date(a, b); }
    if (strcmp(sign, ">=") == 0) { return Is_Greater_date(a, b) || Is_Equal_date(a, b); }
    Error = true;
    ErrorString = (char*)"Неизвестная операция сравнения";
    return false;
}

bool Is_Less_time(time a, time b) { //Время меньше?
    if (a.hour < b.hour) return true;
    if (a.hour > b.hour) return false;
    if (a.minute <= b.minute) return true;
    if (a.minute > b.minute) return false;
    return false;
}
bool Is_Greater_time(time a, time b) { //Время больше?
    if (a.hour > b.hour) return true;
    if (a.hour < b.hour) return false;
    if (a.minute >= b.minute) return true;
    if (a.minute < b.minute) return false;
    return false;
}
bool Is_Equal_time(time a, time b) { //Времена равны?
    return a.hour == b.hour && a.minute == b.minute;
}
bool Compare_time(time a, time b, char sign[3]) { //Сравнить время
    if (strcmp(sign, "<") == 0) { return Is_Less_time(a, b); }
    if (strcmp(sign, ">") == 0) { return Is_Greater_time(a, b); }
    if (strcmp(sign, "==") == 0) { return Is_Equal_time(a, b); }
    if (strcmp(sign, "!=") == 0) { return !Is_Equal_time(a, b); }
    if (strcmp(sign, "<=") == 0) { return Is_Less_time(a, b) || Is_Equal_time(a, b); }
    if (strcmp(sign, ">=") == 0) { return Is_Greater_time(a, b) || Is_Equal_time(a, b); }
    Error = true;
    ErrorString = (char*)"Неизвестная операция сравнения";
    return false;
}

//Сравнить погоду на равенство (отношение больше-меньше не определено)
bool Compare_weather(enum Weather a, enum Weather b, char sign[3]) {
    if (strcmp(sign, "==") == 0) return a == b;
    if (strcmp(sign, "!=") == 0) return a != b;
    Error = true;
    ErrorString = (char*)"Неприменимая операция сравнения";
    return false;
}

//Проверка на вхождение во множество
bool Check_in(Data d, Condition c) {
    int startindex = 1; //квадратную скобку в c.Constant пропустить
    int stopindex;
    while (true) {
        //Выделить очередной элемент множества
        char substring[20];
        stopindex = startindex + 1;
        while (c.Constant[stopindex] != ',' && c.Constant[stopindex] != ']') stopindex++;
        //преобразовать в "погоду"
        for (int i = startindex; i < stopindex; i++)
            substring[i - startindex] = c.Constant[i];
        substring[stopindex - startindex] = 0;
        startindex = stopindex + 1;
        enum Weather w = set_weather_by_text(substring);
        //если совпала - то вернуть true
        if (d.weather == w) { return true; }
        if (c.Constant[stopindex] == ']') { break; }//Множество кончилось
    }
    return false; // в случае отсутствия хотя бы одного совпадения возвращает false
}

bool Sort2 = false;
bool now_sort2 = false;

Data last_d2;


bool Compare(Data d, Condition c)
//Сравнить поле номер "field" данных "d" с использованием условия "c"
{
    int index = c.FieldIndex;
    if (c.Sign[0] == '/') {
        if (strlen(c.Sign) == 4) //Это in
            return Check_in(d, c);
        else
        if (strlen(c.Sign) == 9) //Это include
            return Check_in(d, c);
        else {
            Error = true;
            //strcpy(ErrorString, "Неверный оператор");
            return false;
        }
    }

    switch (index) // вызов функций для сравнения
    {
        case 0: //int geo_id;                     // Идентификатор станции
            return Compare_integer(d.geo_id, set_integer_by_text(c.Constant), c.Sign);
        case 1://char geo_pos[255];                 // Размещение станции, адрес, координаты
            return Compare_String(d.geo_pos, c.Constant, c.Sign);
        case 2://date mea_date;     // Дата наблюдений
            //printf("%s", c.Sign);
            return Compare_Date(d.mea_date, set_date_by_text(c.Constant), c.Sign);
        case 3://int level;
            //printf("%d %s %d = %d\n", d.level, c.Sign, set_integer_by_text(c.Constant), Compare_integer(d.level, set_integer_by_text(c.Constant), c.Sign));
            return Compare_integer(d.level, set_integer_by_text(c.Constant), c.Sign);
        case 4://time sunrise;                     // Время восхода солнца
            return Compare_time(d.sunrise, set_time_by_text(c.Constant), c.Sign);
        case 5://Weather weather;     // Общее состояние погоды во время восхода
            return Compare_weather(d.weather, set_weather_by_text(c.Constant), c.Sign);
            break;
        case 6://time sundown;                     // Заход солнца
            return Compare_time(d.sundown, set_time_by_text(c.Constant), c.Sign);
            break;
        default:
            Error = true;
            ErrorString = (char*)"Неизвестное поле в таблице";
            return false;
    }
}

bool Check_conditions(Data d, Condition * Cond) { //проверка условий
    if (Cond == NULL || Cond[0].Sign[0] == 0) {
        Error = true;
        ErrorString = (char*)"Пустой список условий";
        return false;
    }
    int k = 0;
    while (Cond[k].Sign[0] != 0) {
        Cond[k].FieldIndex = Index_of_Field(Cond[k].FieldName);
        if (Cond[k].FieldIndex < 0) {
            Error = true;
            ErrorString = (char*)"Неизвестное поле";
            return false;
        }
        if (!Compare(d, Cond[k])) return false; //Одно из условий не выполнено
        k++;
    }
    return true; //Выполнены все условия
}

char outbuffer[255];

char * Date_to_string(date value) // запись даты в строку (массив)
{
    memset(outbuffer, 0, sizeof(outbuffer));
    outbuffer[0] = (value.day / 10) + '0';
    outbuffer[1] = (value.day % 10) + '0';
    outbuffer[2] = '.';
    outbuffer[3] = (value.month / 10) + '0';
    outbuffer[4] = (value.month % 10) + '0';
    outbuffer[5] = '.';
    sprintf(&outbuffer[6], "%d", value.year); //заносим в массив год
    return outbuffer;
}
char * Time_to_string(time value) // запись времени в массив
{
    outbuffer[0] = (value.hour / 10) + '0';
    outbuffer[1] = (value.hour % 10) + '0';
    outbuffer[2] = ':';
    outbuffer[3] = (value.minute / 10) + '0';
    outbuffer[4] = (value.minute % 10) + '0';
    outbuffer[5] = 0;
    return outbuffer;
}
char * Weathr_to_string(enum Weather value)
{
    return WeatherName[value];
}


char * Field_to_string(Data d, int field)
{
    switch (field)
    {
        case 0:
            sprintf(outbuffer, "%d", d.geo_id);
            break;
        case 1:
            sprintf(outbuffer, "\"%s\"", d.geo_pos);
            break;
        case 2:
            Date_to_string(d.mea_date);
            break;
        case 3:
            sprintf(outbuffer, "%d", d.level);
            break;
        case 4:
            Time_to_string(d.sunrise);
            break;
        case 5:
            sprintf(outbuffer, "%s", Weathr_to_string(d.weather));
            break;
        case 6:
            Time_to_string(d.sundown);
            break;
    }
    return outbuffer;
}
void Print(FILE * F, Data d) {
    for (int field = 0; field < 7; field++)
        fprintf(F, "%s=%s ", FieldName[field], Field_to_string(d, field));
    fprintf(F, "\n");
}

bool Sravnenie_zapisey(Data a, Data b, Values  * V) { //Сравнить две записи
    Condition C;
    int K = 0;
    while (true) { //Пока есть что сравнивать
        strcpy(C.Sign, "=="); //Проверить на равенство сначала
        strcpy(C.FieldName, V[K].fieldname);
        C.FieldIndex = Index_of_Field(V[K].fieldname);
        strcpy(C.Constant, Field_to_string(b, C.FieldIndex));
        if (Compare(a, C)) { //если равны
            K++;
            if (V[K].fieldname[0] == 0) return false; //Полное равенство
            continue;
        }
        if (V[K].value[0] == 'd' && V[K].value[1] == 'e' &&V[K].value[2] == 's' &&V[K].value[3] == 'c')
        {
            //printf("bla1\n");
            strcpy(C.Sign, ">");
        }
        if (V[K].value[0] == 'a' && V[K].value[1] == 's' &&V[K].value[2] == 'c') {
            //printf("bla2\n");
            strcpy(C.Sign, "<");
        }
        //и возвращается результат сравнения
        return Compare(a, C);
    }
}
///_______________Конец_Работы_С_Данными_______________///


///_______________Необходимое_Для_Работы_Со_Списком_______________///
typedef struct List {
    Data d; //Сами данные
    struct List* next; //Ссылка на следующий
    bool mark; //Пометка на удаление
} List;

List * Insert(Data d, List * Table) { //Добавить в список новые данные
    if (!Table) {
        Table = (List*)malloc(sizeof(List));
        Table[0].d = d;
        Table[0].next = NULL;
        stat.s_malloc++; //Статистику
        return Table;
    }
    List * current = Table;
    while (current->next) current = current->next; //позиция в конце списка
    //Вставить
    List * l = (List*)malloc(sizeof(List));
    stat.s_malloc++; //Статистику
    l->d = d;
    l->next = NULL;
    current->next = l;
    return Table;
}

List * Delete(List * Table, Condition * Cond) { //Удалить из списка запись
    List * current = Table;
    while (current) {
        current->mark = Check_conditions(current->d, Cond);
        current = current->next;
    }
    while (Table->mark) {
        current = Table->next; //Кто следующий?
        free(Table); //Освободить память
        stat.s_free++;
        Table = current; //Перейти к следующему
        if (!Table) return NULL; //Просто все удалились ((
    }
    current = Table; //Точно не удаляемый.
    while (current->next) { //Пока не кончится таблица
        if (current->next->mark) {//Если следующий помечен на удаление
            List * temp = current->next; //Запомнить, кого удалить
            current->next = current->next->next; //Замкнуть ссылку
            free(temp); //Удалить
            stat.s_free++; //Статистика
        }
        else
            current = current->next; //если не помечен - переход к следующему
    }
    return Table;
}

List * Select(List * Table, Condition * Cond) { //поиск записей в БД
    List * result = NULL;
    List * current = Table;
    if (Cond == NULL) {//Если select оказался пустым
        while (current) {
            result = Insert(current->d, result);
            current = current->next;
        }
    }
    else {
        while (current) {
            if (Check_conditions(current->d, Cond))
                result = Insert(current->d, result); //выводит результат
            current = current->next;
        }
    }
    return result;
}

Data UpdateData(Data d, Values * V, int * count) {
    *count = 0;
    if (!V) return d; //Нет данных
    //Для всего массива value
    while (V[*count].fieldname[0] != 0) {
        int index = Index_of_Field(V[*count].fieldname); //найти поле
        date dat = set_date_by_text(V[*count].value);
        time t = set_time_by_text(V[*count].value);
        switch (index) //установить новое значение
        {
            case 0://geo_id
                d.geo_id = set_integer_by_text(V[*count].value);
                break;
            case 1://geo_pos
            {
                if ((V[*count].value[0]) == '\"') { //Если первый символ - кавычка, то устранить
                    char temp[255];
                    strcpy(temp, &V[*count].value[1]);
                    temp[strlen(temp) - 1] = 0;
                    strcpy(d.geo_pos, temp);
                }
                else //иначе копировать как есть
                    strcpy(d.geo_pos, V[*count].value);
            }
                break;
            case 2://mea_date
                d.mea_date = dat;
                break;
            case 3://level
                d.level = set_integer_by_text(V[*count].value);
                break;
            case 4://sunrise
                d.sunrise = t;
                break;
            case 5://weather
                d.weather = set_weather_by_text(V[*count].value);
                break;
            case 6://sundown
                d.sundown = t;
                break;
            default:
                Error = true;
                ErrorString = (char*)"Неизвестное поле";
                break;
        }
        (*count)++; //Следующее
    }
    return d;
}
void Update(List * Table, Condition * Cond, Values * value, int * Updated) {//обновляет указанные поля на указанные значения
    List * current = Table;
    int count;
    *Updated = 0;
    while (current) {
        if (Check_conditions(current->d, Cond)) {// если условия совпадают, то происходит обновление указанного поля на указанное значение
            current->d = UpdateData(current->d, value, &count);
            (*Updated)++;
        }
        current = current->next;
    }
}

List * Pop(List * Table) {//Удалить первый элемент таблицы
    if (!Table) return NULL;
    List * result = Table->next;
    free(Table);
    stat.s_free++;
    return result;
}
void Clear_all_list(List * Table) {//Удалить список полностью
    while (Table) { Table = Pop(Table); } //Пока не очистится - удалять первый элемент
}

Values *old_V;

List * Sortirovka(List * Table, Values * V) {//Сортировка слиянием (модифицированным для списков)
    //Не пора ли закончить?
    if (!Table) { return NULL; } //Это вообще пустой список
    if (Table->next == NULL) { return Table; } //Это список из одного элемента. Он отсортирован.


    bool ToFirst = false; //переменная для разделения списка
    List * Part1 = NULL; // Разбить список пополам
    List * Part2 = NULL;
    List * current = Table;
    while (current) {
        ToFirst = !ToFirst; //По очереди
        if (ToFirst) { Part1 = Insert(current->d, Part1); }
        else { Part2 = Insert(current->d, Part2); }
        current = current->next; //переходим к ледующему
    }
    Clear_all_list(Table); //Удалить исходный список

    Part1 = Sortirovka(Part1, V); //Вызвать сортировку каждой части
    Part2 = Sortirovka(Part2, V);
    //Слить полученные части, используя условие сортировки
    List * result = NULL;
    List * part1 = Part1;
    List * part2 = Part2;
    while (part1 || part2) { //Пока есть данные хоть в одном из списков
        if (!part1) { //Первый список пуст
            result = Insert(part2->d, result); //Добавить данные из второго списка
            part2 = part2->next;
            continue; //и продолжить
        }
        if (!part2) { //Второй список пуст
            result = Insert(part1->d, result);
            part1 = part1->next;
            continue;
        }
        if (Sravnenie_zapisey(part1->d, part2->d, V)) {
            //Меньше - из первого списка
            result = Insert(part1->d, result);
            part1 = part1->next;
        }
        else {
            //Меньше - из второго списка.
            result = Insert(part2->d, result);
            part2 = part2->next;
        }
    }
    Clear_all_list(Part1);//Очищаем ненужное
    Clear_all_list(Part2);
    return result;
}

bool Check_data(Data d1, Data d2, Values *V) {
    char c[3] = "==";
    if (strstr(V->fieldname, "geo_id")) {
        return (d1.geo_id == d2.geo_id);
    }
    if (strstr(V->fieldname, "level")) {
        return (d1.level == d2.level);
    }
    if (strstr(V->fieldname, "mea_date")) {
        return (Compare_Date(d1.mea_date, d2.mea_date, c));
    }
    if (strstr(V->fieldname, "sunrise")) {
        return (Compare_time(d1.sunrise, d2.sunrise, c));
    }
    if (strstr(V->fieldname, "sundown")) {
        return (Compare_time(d1.sundown, d2.sundown, c));
    }
}
List * Sor(List * Table, Values *old_V, Values * V)
{
    List *result = NULL;
    while (Table != NULL)
    {
        Data control = Table->d;
        List* sort_Table = NULL;
        sort_Table = Insert(Table->d, sort_Table);
        printf("%d SUNRISE\n", Table->d.sunrise);
        Table = Table->next;
        int count = 0;
        while (Check_data(Table->d, control, old_V)) {
            count++;
            sort_Table = Insert(Table->d, sort_Table);
            Table = Table->next;
            if (Table == NULL) {
                break;
            }
        }
        if (sort_Table != NULL) {
            sort_Table = Sortirovka(sort_Table, V);
            while (sort_Table != NULL) {
                result = Insert(sort_Table->d, result);
                sort_Table = sort_Table->next;
            }
        }
        else {
            result = Insert(Table->d, result);
        }
        printf("%d - COUNTBLYAT\n", count);

    }
    return result;
}

int Length(List * Table) { //считаем элементы
    List * current = Table;
    int result = 0;
    while (current) {
        current = current->next;
        result++;
    }
    return result;
}

bool isEqual(Data d1, Data d2, Values * Fields) {
    int K = 0;
    Condition C;
    strcpy(C.Sign, "=="); //Проверять на равенство
    while (Fields[K].fieldname[0]) {
        strcpy(C.FieldName, Fields[K].fieldname); //Задать имя поля для сравнения
        C.FieldIndex = Index_of_Field(Fields[K].fieldname); //Номер поля
        strcpy(C.Constant, Field_to_string(d2, C.FieldIndex)); //Константа для сравнения
        if (!Compare(d1, C)) { return false; } //Не совпали!
        K++;
    }
    return true; //Все проверки пройдены
}
bool IsUnique(Data d, List * Table, Values * Fields) { //true, если в таблице нет похожих
    if (!Table) { return true; } //В пустых данных все уникально
    while (Table)
        if (isEqual(d, Table->d, Fields)) { return false; }
        else { Table = Table->next; }
    return true;
}
List * Unique(List * Table, Values * Fields) { //удаление дубликатов
    if (!Table) { return NULL; }
    List * result = NULL;

    while (Table) { //пока не кончится таблица
        if (IsUnique(Table->d, Table->next, Fields))
            result = Insert(Table->d, result);
        Table = Pop(Table);
    }
    return result; //вернуть результат
}
///_______________Конец_Работы_Со_Списком_______________///


///_______________Необходимое_Для_Работы_С_Синтаксисом_______________///
//создаем список
bool Make_list(char * buffer, Values ** ValueList, Condition ** ConditionList, bool nosign) {
    int N = strlen(buffer); //общая длина входной строки
    buffer[N++] = ' '; //Добавить пробел в конец встроки
    buffer[N++] = 0;
    Values * V = NULL; //Приемник для значений
    Condition * C = NULL; //Приемник для условий
    if (*ValueList) { free(*ValueList); *ValueList = NULL; stat.s_free++; }; //Освобождается массив
    if (*ConditionList) { free(*ConditionList); *ConditionList = NULL; stat.s_free++; }; //Освобождается массив
    int NVL = 0;//Количество элементов в массиве
    int NCL = 0;

    int finish = 0; //Конец лексемы
    while (buffer[finish] == ' ' && finish < N) { finish++; } //пропустить пробелы
    if (finish == N) {
        Error = true;
        if (!Error)
            ErrorString = (char*)"Команда без параметров";
        return false;
    }
    int sign = 0; //текущий символ на строке
    int start = 0; //начало лексемы
    bool slash = false; //начало "/"
    //Для всего буфера
    while (finish<N) {//начинаем смотреть входную строку
        while (buffer[finish] == ' ' && finish < N) { finish++; } //Пропустить пробелы
        //Выделить лексему (между пробелами (или запятыми), которые не внутри кавычек (или внутри))
        start = finish;
        sign = -1;
        bool Quote = false; //Признак "кавычка"
        bool SquareBrackets = false; ///Признак "квадратная скобка"
        while (finish < N) {
            if (buffer[finish] == '"') { Quote = !Quote; }
            if (Quote) { finish++; continue; }
            ///Обработка открывающей и закрывающей скобок
            if (SquareBrackets && buffer[finish] == ']') { SquareBrackets = false; }
            if (SquareBrackets) { finish++; continue; }
            if (buffer[finish] == '[') { SquareBrackets = true; }
            ///Обработка пары символов /
            if (buffer[finish] == '/') {
                if (slash) {
                    slash = false;//Это второй /
                    //break;
                }
                else {
                    slash = true; //Это начало знака операции /in/ или /include/
                    sign = finish;
                }
            }
            if (buffer[finish] == ' ' || buffer[finish] == ',') break; //Нашелся конец лексемы
            //Узнать, где находится первый знак операции (= или сравнение)
            if (sign < 0 && (buffer[finish] == '<' || buffer[finish] == '>' || buffer[finish] == '=')) { sign = finish; }
            /*if (sign < 0 && (buffer[finish] == '/')) { sign = finish; }*/
            if (sign < 0 && (buffer[finish] == '!')) { sign = finish; }
            finish++;
        };

        if (Quote) { //ковычка
            Error = true;
            if (!Error)
                ErrorString = (char*)"Незакрытая кавычка";
            if (V) { free(V); stat.s_free++; };
            if (C) { free(C); stat.s_free++; };
            return false;
        }
        if (SquareBrackets) { //квадратная скобка
            Error = true;
            if (!Error)
                ErrorString = (char*)"Квадратная скобка не закрыта";
            if (V) { free(V); stat.s_free++; };
            if (C) { free(C); stat.s_free++; };
            return false;
        }
        if (slash) {
            Error = true;
            if (!Error)
                ErrorString = (char*)"Незакрытый /";
            if (V) { free(V); stat.s_free++; };
            if (C) { free(C); stat.s_free++; };
            return false;
        }

        if (nosign) { sign = finish; };//Блокировка формата field=value, для uniq

        if (sign < 0) {
            if (finish - start > 2)
                Error = true;
            if (Error)

                ErrorString = (char*)"Нет знака операции";

            finish++;
            continue;
        };

        //Распознать
        //определить тип
        bool isField = false;
        int signlength = 2; //по умолчанию
        //Знаком может быть
        //=
        if (buffer[sign] == '=') //проверка на ==
            if (buffer[sign + 1] != '=')
                isField = true;//Это описание поля
        //<
        if (buffer[sign] == '<')//проверка на строгость неравенства (<> или <=)
            if (buffer[sign + 1] != '=' && buffer[sign + 1] != '>')
                //Это тоже сравнение, но длина 1
                signlength = 1;
        //>
        if (buffer[sign] == '>')
            //или >=
            if (buffer[sign + 1] != '=')
                signlength = 1;

        if (buffer[sign] == '/') {
            ///это или in, или include
            signlength = 2;
            while (buffer[sign + signlength] != '/') signlength++;
            signlength++;
        }

        if (nosign) {
            signlength = 1; //Чему угодно
            sign = finish; //Неважно где
            isField = true; //Это в любом случае имя поля
        }

        //если это поле
        if (isField) {
            V = (Values*)realloc(V, (NVL + 1) * sizeof(Values)); //Выделить память
            memset(&V[NVL], 0, sizeof(Values));
            if (NVL == 0) stat.s_malloc++; else stat.s_realloc++;
            memcpy(V[NVL].fieldname, &buffer[start], sign - start); //скопировать имя поля
            if (!nosign) { memcpy(V[NVL].value, &buffer[sign + 1], finish - sign - 1); } //скопировать константу (если она есть)
            NVL++;
        }
            //если это условие
        else {
            C = (Condition*)realloc(C, (NCL + 1) * sizeof(Condition));
            memset(&C[NCL], 0, sizeof(Condition));
            if (NCL == 0) { stat.s_malloc++; }
            else { stat.s_realloc++; }
            memcpy(C[NCL].FieldName, &buffer[start], sign - start);	//скопировать имя поля
            memcpy(C[NCL].Sign, &buffer[sign], signlength);//скопировать знак операции
            if (strcmp(C[NCL].Sign, "!=") == 0) { strcpy(C[NCL].Sign, "<>"); }
            memcpy(C[NCL].Constant, &buffer[sign + signlength], finish - (sign + signlength)); //скопировать константу

            ///Добавлено
            if (C[NCL].Sign[0] == '/') { Error = false; } //Проверка будет на этапе выполнения
            NCL++;
        }
    }

    if (NVL != 0) {
        V = (Values*)realloc(V, (NVL + 1) * sizeof(Values));
        stat.s_realloc++;
        memset(&V[NVL], 0, sizeof(Values));
    }
    if (NCL != 0) {
        C = (Condition*)realloc(C, (NCL + 1) * sizeof(Condition));
        stat.s_realloc++;
        memset(&C[NCL], 0, sizeof(Condition));
    }
    *ValueList = V;
    printf("%s\n", V);
    *ConditionList = C;
    return true;
}

char PossibleCommands[][10] = { "insert","select","delete","update","uniq","sort" };//возможные команды
void Extract_Call(char * InFileName, char * OutFileName, List * Table) {
    Input = fopen(InFileName, "r"); 	//Открыть входной и выходной файлы
    Output = fopen(OutFileName, "w");
    Detail = fopen("Detail.txt", "w");
    //По одной строке читать команды
    Values *V = NULL;
    Condition *C = NULL;

    while (!feof(Input)) {//Если файл кончился завершаем цикл
        bool STOP = false;
        char buffer[1024];
        fgets(buffer, 1024, Input);

        buffer[strlen(buffer)] = 0;
        fprintf(Detail, "read:%s\n", buffer);

        if (buffer[0] == 0) { continue; } //Пропустить пустую строку
        if (strlen(buffer) >= 2 && buffer[0] == '-' && buffer[1] == '-') { continue; } //Комментарий в sql- стиле начинается с двух тире, пропускаем
        char command[20]; //для определения команды

        for (int k = 0; k < sizeof(command); k++) {
            if (buffer[k] == ' ') { break; }
            else { command[k] = buffer[k]; }

            command[k + 1] = 0;
            fprintf(Detail, "command:%s\n", command);
            //Определение команды
            int commandcode = -1;
            for (int k = 0; k < 6; k++)
                if (strcmp(PossibleCommands[k], command) == 0) { commandcode = k; }
            //В зависимости от команды выделить параметры и Выполнить команду
            fprintf(Detail, "command code:%d\n", commandcode);

            Error = false; //Допустим строка в файле введена без ошибок
            //printf("%s - VALUE\n", V);
            Make_list(&buffer[strlen(PossibleCommands[commandcode])], &V, &C, commandcode == 4);
            //printf("%s - VALUE\n", V->fieldname);
            //printf("%s - Va")
            if ((buffer[0] != 's' || buffer[1] != 'e' || buffer[2] != 'l' || buffer[3] != 'e' || buffer[4] != 'c' || buffer[5] != 't') ||
                (buffer[0] != 'd' || buffer[1] != 'e' || buffer[2] != 'l' || buffer[3] != 'e' || buffer[4] != 't' || buffer[5] != 'e')) {
                Error = false;
            }
            if (Error) {
                fprintf(Detail, "Error %s\n", ErrorString);
                if (commandcode < 0) {
                    fprintf(Output, "incorrect:");
                    for (unsigned int k = 0; k < 20 && k < strlen(buffer); k++) {
                        fprintf(Output, "%c", buffer[k]);
                    }
                    fprintf(Output, "\n");
                }
                continue;
            }

            //Для детальной проверки утечки памяти
            fprintf(Detail, "malloc - free:%d\n", stat.s_malloc - stat.s_free);

            switch (commandcode)
            {
                case 0:  //insert - вставка
                {
                    fprintf(Detail, "insert\n");
                    // Добавить данные.
                    //Если длина C > 0 то что-то пошло не так
                    if (C != NULL && C[0].FieldName[0] != 0) {
                        fprintf(Detail, "Обнаружено условие\n");
                        break;
                    }
                    //Для всех V занести данные, подсчитывая количество полей
                    Data d = { 0 };
                    int index = 0;
                    int count = 0;
                    while (V[count].fieldname[0] != 0) {
                        date dat = set_date_by_text(V[count].value);
                        time t = set_time_by_text(V[count].value);
                        if (!Check_date(dat)) {
                            STOP = true;
                            fprintf(Output, "Некорректные данные даты: = %d, %d, %d; \n", dat.day, dat.month, dat.year);
                            break;
                        }
                        if (!Check_time(t)) {
                            STOP = true;
                            fprintf(Output, "Некорректные данные времени: = %d, %d; \n", t.hour, t.minute);
                            break;
                        }
                        count++;
                    }
                    if (!STOP) {
                        d = UpdateData(d, V, &index);

                        //Если количество полей неверное, отказаться от вставки,
                        if (index != 7) {
                            fprintf(Detail, "Не все данные\n");
                            fprintf(Output, "incorrect:'\n");
                            for (unsigned int k = 0; k < 20 && k < strlen(buffer); k++)
                                fprintf(Output, "%c", buffer[k]);
                            fprintf(Output, "'\n");
                            break;
                        }
                        //иначе - добавить к данным
                        Table = Insert(d, Table);
                        fprintf(Output, "insert:%d\n", Length(Table));
                    }
                }
                    break;
                case 1: //select - поиск записей в БД
                {
                    printf("YA TUT\n");
                    fprintf(Detail, "select\n");

                    //Осуществить выборку
                    List * Result = Select(Table, C);;

                    fprintf(Output, "select:%d\n", Length(Result));
                    //Вывести все записи result в виде field = xxx
                    List * current = Result;
                    while (current) {
                        Print(Output, current->d);
                        current = current->next;
                    }
                    //Удалить результат
                    Clear_all_list(Result);
                }
                    break;
                case 2: //delete
                {
                    fprintf(Detail, "delete\n");
                    int Before = Length(Table);
                    Table = Delete(Table, C); //Удалить записи
                    int After = Length(Table);
                    if (C != NULL) {
                        fprintf(Output, "delete:%d\n", Before - After);
                    }
                    else {
                        List *current = Table;
                        while (current) {
                            Table = current;
                            current = Table->next;
                            free(Table);
                            stat.s_free++;
                        }
                        Table = current;
                        fprintf(Output, "delete:%d\n", Before);
                    }
                }
                    break;
                case 3: //update
                {
                    fprintf(Detail, "update\n");
                    int Updated = 0;
                    int count = 0;
                    while (V[count].fieldname[0] != 0) {
                        date dat = set_date_by_text(V[count].value);
                        time t = set_time_by_text(V[count].value);
                        if (!Check_date(dat)) {
                            STOP = true;
                            fprintf(Output, "Некорректные данные даты: = %d, %d, %d; \n", dat.day, dat.month, dat.year);
                            break;
                        }
                        if (!Check_time(t)) {
                            STOP = true;
                            fprintf(Output, "Некорректные данные времени: = %d, %d; \n", t.hour, t.minute);
                            break;
                        }
                        count++;
                    }
                    if (!STOP) {
                        Update(Table, C, V, &Updated);//Обновить данные
                        fprintf(Output, "update:%d\n", Updated);
                    }
                }
                    break;
                case 4: //uniq - удаление дубликатов
                {
                    fprintf(Detail, "uniq\n");
                    int Before = Length(Table);
                    Table = Unique(Table, V);//Удалить повторяющиеся записи
                    int After = Length(Table);
                    fprintf(Output, "uniq:%d\n", Before - After);
                }
                    break;
                case 5: //sort - сортировка
                {
                    fprintf(Detail, "sort\n");
                    printf("fieldname: %s\n", V->fieldname);
                    printf("value: %s\n", V->value);
                    printf("size_buffer %d |%s", strlen(buffer), buffer);
                    char *res_check = strstr(buffer, V->value);

                    printf("%s RES_CHECK\n", res_check);
                    printf("%d LENG\n", strlen(res_check));
                    int leng = strlen(V->value);
                    if (res_check[4 + leng + 2] != 0 && res_check[4 + leng + 1] != 0 && res_check[4 + leng] != 0) {
                        char *s = strstr(buffer, V->value);
                        printf("%s BUFFER\n", s);
                        int c = 0;
                        char *d = (char*)malloc(sizeof(char));
                        char *n = (char*)malloc(sizeof(char));
                        if ((n = strstr(s, "geo_id="))) {
                            d = strstr(s, "geo_id=");
                        }
                        else if ((n = strstr(s, "mea_date="))) {
                            d = strstr(s, "mea_date=");
                        }
                        else if ((n = strstr(s, "level="))) {
                            d = strstr(s, "level=");
                        }
                        else if ((n = strstr(s, "sunrise="))) {
                            d = strstr(s, "sunrise=");
                        }
                        else if ((n = strstr(s, "sundown="))) {
                            d = strstr(s, "sundown=");
                        }

                        c = 0;
                        int c1 = 0;
                        char *field = (char*)malloc(10 * sizeof(char));
                        char *val = (char*)malloc(5 * sizeof(char));
                        while (d[c] != '=') { field[c] = d[c++]; }
                        printf("%d - C\n", c);
                        field[c] = 0;
                        while (d[c1 + c + 1] >= 97) { val[c1] = d[c1 + c + 1]; c1++; }
                        printf("%d - C | %d - C1\n", c, c1);
                        val[c1] = 0;

                        old_V = (Values*)malloc(sizeof(Values));

                        strcpy(old_V->fieldname, V->fieldname);
                        strcpy(old_V->value, V->value);

                        printf("RES: %s\n", d);
                        printf("field: %s\n", field);
                        printf("val: %s\n", val);

                        printf("TEST");
                        Table = Sortirovka(Table, V); //Сортировать данные


                        strcpy(V->fieldname, field);
                        strcpy(V->value, val);


                        Table = Sor(Table, old_V, V);
                    }
                    else {
                        fprintf(Detail, "sort\n");
                        old_V = NULL;
                        Table = Sortirovka(Table, V);
                    }
                    fprintf(Output, "sort:%d\n", Length(Table));
                }
                    break;
                default: //Не опознана
                {
                    if (k == sizeof(command) - 1) {
                        fprintf(Detail, "incorrect\n");
                        fprintf(Output, "incorrect:");
                        for (unsigned int k = 0; k < 20 && k < strlen(buffer); k++) {
                            fprintf(Output, "%c", buffer[k]);
                        }
                        fprintf(Output, "\n");
                    }
                }
                    break;
            }
        }
    }
    //Закончить
    fclose(Input);
    Clear_all_list(Table); //Освободить память

    if (V) { free(V); stat.s_free++; }
    if (C) { free(C); stat.s_free++; }

    fprintf(Output, "malloc:%d\n", stat.s_malloc); //Выводим статистику
    fprintf(Output, "realloc:%d\n", stat.s_realloc);
    fprintf(Output, "calloc:%d\n", 0);
    fprintf(Output, "free:%d\n", stat.s_free);
    fclose(Output); fclose(Detail);
}
///_______________Конец_Работы_С_Синтаксисом_______________///

Data * data = NULL; //Данные
int main() {
    List * Table = NULL;
    Extract_Call((char*)"Input.txt", (char*)"Output.txt", Table);
    system("pause");

}
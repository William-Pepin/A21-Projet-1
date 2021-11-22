struct Drugs
{
    short red;
    short bleu;
    short green;
};

struct Patient
{
    char first_name[50];
    char last_name[50];
    short room_number;
    char rfid_code[50];
    struct Drugs daily_drugs;
    bool daily_drugs_confirmation;
};
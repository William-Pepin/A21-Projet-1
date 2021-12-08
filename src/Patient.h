struct Drugs
{
    int red;
    int green;
    int blue;
    int yellow;
};

struct Patient
{
    char first_name[32];
    char last_name[32];
    int room_number;
    char rfid_code[50];
    struct Drugs dailydrugs;
    bool distributed;
};

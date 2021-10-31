#include <LibRobus.h>
#include <math.h>

bool StateSignalSonore=false;
bool StateQuilleTombee=false;
bool StateCrossroad=false;
int StateCouleur=0; //noir=0, blanc=1, rouge=2, etc.
float AnglePIDGauche=20;
float AnglePIDDroit=340;
int StateDirection=0; 
int Timer=0;

void setup()
{
  BoardInit();
  delay(100);
}

void loop()
{

    if(fonction MesureSonore=true){
        StateSignalSonore=true;
    }

    if(StateSignalSonore=false){
        //fonction MesureSuiveur
        //fonction AjustementDirection
    }

    if(StateSignalSonore=true){
        if(StateQuilleTombee=false){
            //Fonction balayage et infrarouge
            //Fonction avancer et renverser quille
            //Fonction reculer à la position de départ
            //Fonction rotation jusqu'à ce que StateDirection=1
        }
        if(StateQuilleTombee=true){
            if(StateCrossroad=false){
                //Fonction suiveur de ligne
                //Fonction détection crossroad
            }
            if(StateCrossroad=true){
                //Fonction aller au point couleur
                StateCouleur=Mesurer la couleur
                if(StateCouleur=0){
                    //Apporter balle au point noir
                }
                else if(StateCouleur=1){
                    //Apporter balle au point blanc
                }
                else if(StateCouleur=2){
                    //Apporter balle au point rouge
                }
            }
        }
    }
}

void mesuresuiveur(){
    if (Timer>=100){
        //prendre mesure du suiveur=mesure

        /*
        Mesure de tension du capteur:   V1 (rouge)   |   V2 (jaune)   |   V3 (rouge)   |   V4 (bleu)   |   Mesure (V):
        1                                                                                                    0,002
        2                                    X                                                               2,534
        3                                                    X                                               1,271
        4                                                                    X                               0,649
        5                                                                                    X               0,307
        6                                    X               X                                               3,721
        7                                                    X               X                               1,877
        8                                                                    X               X               0,948
        9                                    X                               X                               3,129
        10                                                   X                               X               1,549
        11                                   X                                               X               2,795
        12                                   X               X               X                               4,360
        13                                                   X               X               X               2,187
        14                                   X                               X               X               4,084
        15                                   X               X                               X               3,473
        16                                   X               X               X               X               4,648

        Mesures triées: 0,002; 0,307; 0,649; 0,948; 1,271; 1,549; 1,877; 2,187; 2,534; 2,795; 3,129; 3,473; 3,721; 4,084; 4,360; 4,648
        */

        double mesure=analogRead(A6)/200.8;
        if(mesure<0.15){
            StateDirection=1;
        }
        else if(mesure>=0.15 && mesure<0.45){
            StateDirection=5;
        }
        else if(mesure>=0.45 && mesure<0.8){
            StateDirection=4;
        }
        else if(mesure>=0.8 && mesure<1.1){
            StateDirection=8;
        }
        else if(mesure>=1.1 && mesure<1.4){
            StateDirection=3;
        }
        else if(mesure>=1.4 && mesure<1.7){
            StateDirection=10;
        }
        else if(mesure>=1.7 && mesure<2){
            StateDirection=7;
        }
        else if(mesure>=2 && mesure<2.3){
            StateDirection=13;
        }
        else if(mesure>=2.3 && mesure<2.65){
            StateDirection=2;
        }
        else if(mesure>=2.65 && mesure<2.95){
            StateDirection=11;
        }
        else if(mesure>=2.95 && mesure<3.25){
            StateDirection=9;
        }
        else if(mesure>=3.25 && mesure<3.6){
            StateDirection=15;
        }
        else if(mesure>=3.6 && mesure<3.9){
            StateDirection=6;
        }
        else if(mesure>=3.9 && mesure<4.2){
            StateDirection=14;
        }
        else if(mesure>=4.2 && mesure<4.5){
            StateDirection=12;
        }
        else if(mesure>=4.5){
            StateDirection=16;
        }
    }
}

void AjustementDirection(){
    if(ENCODER_Read(0)<=250 && ENCODER_Read(1)<=250){
        ENCODER_Reset(0);
        ENCODER_Reset(1);
        if(StateDirection=0 || StateDirection=3 || StateDirection=10 || StateDirection=13){
            MOTOR_SetSpeed(0,0.2);
            MOTOR_SetSpeed(1,0.2);
        }
        if(StateDirection=2){
            MOTOR_SetSpeed(0,0.1);
            MOTOR_SetSpeed(1,0.2);
        }
        if(StateDirection=4){
            MOTOR_SetSpeed(0,0.2);
            MOTOR_SetSpeed(1,0.1);
        }
    }
}
#include <LibRobus.h>
#include <math.h>

bool StateSignalSonore=false;
bool StateQuilleTombee=false;
bool StateCrossroad=false;
int StateCouleur=0; //noir=0, blanc=1, rouge=2, etc.
float AnglePIDGauche=20;
float AnglePIDDroit=340;
int StateDirection=1;  //0:ajustement à gauche, 1:Tout droit, 2:Ajustement à droite


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
    if (ENCODER_ReadReset(0)>1000 && ENCODER_ReadReset(1)>1000){
        //prendre mesure du suiveur=mesure
        if(mesure=x){
            StateDirection=1;
        }
        else if(mesure=y){
            StateDirection=0;
        }
        else if(mesure=z){
            StateDirection=2;
        }
    }
}

void AjustementDirection(){
    if(StateDirection=0){
        Fonction PID avancer avec angle de 20 degré vers la gauche
    }
    if(StateDirection=1){
        Fonction PID avancer droit
    }
    if(StateDirection=2){
    Fonction PID avancer gauche
    }
}
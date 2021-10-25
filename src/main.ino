#include <LibRobus.h>
#include <math.h>

bool StateSignalSonore=false;
bool StateQuilleTombee=false;
bool StateCrossroad=false;
int StateCouleur=0; //noir=0, blanc=1, rouge=2, etc.

void setup()
{
  BoardInit();
  delay(100);
}

void loop()
{


    if(StateSignalSonore=false){
        //fonction suiveur
        if(fonction sonore=True){
            StateSignalSonore=True
        }
    }

    if(StateSignalSonore=True){
        if(StateQuilleTombee=False){
            //Fonction balayage et infrarouge
            //Fonction avancer et renverser quille
            //Fonction reculer à la position de départ
            //Fonction retour sur la ligne
        }
        if(StateQuilleTombee=True){
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

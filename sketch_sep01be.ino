/*
 Detects Secret Ring Pattern and Triggers Diode if pattern is correct
 
 
 //slight changes in this version
 
 arrays werden nciht uebergeben, da global ausserdem ist das array nun eindimensional
 
	by Tobias Hoelzer
	Version 0.1.1
`


	Digital Pin 10 	Ringing Bell Diode Input for Ringing the Bell
        Digital Pin 04 Program Pin to Programm a new Pattern
        Digital Pin 09 Pin to show current pattern
        
	Digital Pin 13	Diode Signaling Ring
	Digital Pin 12 Diode Green to show if pattern is correct
	Digital Pin 08 Diode Red to show if pattern is incorrect or to signal programming mode
        




 */

// constants used here to set pin numbers:



const int RingPin = 2;     // the number of RingPin
const int ledPin =  13;      // the number of the LED pin
const int ProgramPin=11;    //the number of the ProgrammButtonPin
const int greenledPin = 7; //the number of the green LED pin	
const int redledPin = 4;	//the number of the red LED pin
const int ShowPattern = 8;     // the number of RingPin


#define NumberofRings 5
//const int NumberofRings=5;	//max number of Rings possible
//const does not work properly with no apparent reason




//other constants 

const int ringfinished=3000;	//time available to ring again
const int Sensibility=10;        //Sensibility to the right timing





// variables will change:


boolean RingState = false;                               // variable for reading the ringing status, is the ringbutton currently pressed?
int RingArray[NumberofRings];  //variable Array for the incoming ring
int MasterArray[NumberofRings];//variable array for the secret pattern
boolean Codechecker=false;                              //variable for code is correct/incorrect
boolean ProgramState=false;                             //variable for the last state ring/noring
boolean ShowPatternState=false;                         //variable for the showpatternbutton, is the showcurrrentpatternbutton pressed?






void setup() {
  
          //Input Pins
  
          pinMode(RingPin, INPUT);      // initialize the pushbutton pin as an input: 
          pinMode(ProgramPin, INPUT);  //initialize the Program pin as an input
          pinMode(ShowPattern,INPUT);  //initialize the Show Pattern pin as an input     
          
          //Output Pins
  
          pinMode(ledPin, OUTPUT);      // initialize the LED pin as an output: 
          pinMode(greenledPin, OUTPUT); // initialize the  green LED pin as an output:
          pinMode(redledPin, OUTPUT);   // initialize the red LED pin as an output:
           
          
          Serial.begin(9600);          // initialize serial communications at 9600 bps:  
}

void loop(){
  

                          //first, check all the input pins
                       
                        RingState = digitalRead(RingPin);              // read the state of the Ringpin value:
                        
                        ProgramState=digitalRead(ProgramPin);          //read the state of the Programpin value
                        
                        ShowPatternState=digitalRead(ShowPattern);    //read the state of the showpatternpin value
                        
                        
                        //if Show PatternPin is active, show pattern
                        
                        if(ShowPatternState){
                          
                                                 Blink(MasterArray); 
                         
                        }//endofif
                        
                        
                        
                        
                        //if Programstate Pin is active signal it by red LED otherwise not
                        
                        if(ProgramState){
                          
                                                  digitalWrite(redledPin,HIGH);   
                          
                        } else {
                          
                                                   digitalWrite(redledPin,LOW);
                          
                        }//endofif
                        
                        
                        
                        
                        
                      
                        // check if there is a Ring
                        // if it is, the RingState is HIGH:
                                                
                        
                        if (RingState) {     
                          
                                          
                                           ListenToRing();                         //listen to the ringings
                                           MapArray();                            //and map them
                                           
                                           
                                           //now check if program state to save new pattern or usual state to open the door
                                           
                                           if(ProgramState){              //if in PROGRAM STATE
                                           
                                           
                                                              ChangeMasterArray();    // write current pattern into the master Array
                                                              Blink(MasterArray);                          // and show the new pattern
                                              
                                              
                                              
                                           }else{                        //if in USUAL STATE 
                       
                       
                                                          Codechecker=CheckSecretCode(RingArray,MasterArray);  //check if the current pattern matches the Secret Pattern
                                                          
                                                          delay(500);                                          //Wait 500 millis to increase tension
                                                                
                                                       
                                                       
                                                        
                                                        
                                                       
                                                               //now check if the pattern was right
                                                               
                                                          if(Codechecker){  //if code was right blink the green led and reset the codecheck variable to default false;
                                                            
                                                                            digitalWrite(greenledPin,HIGH);
                                                                            
                                                                            delay(100);
                                                                            
                                                                            digitalWrite(greenledPin,LOW);
                                                                            
                                                                
                                                                           Codechecker=false;                          //reset codecheck
                                                                           
                                                                           
                                                          } else{ //if code was not right just blink the red led
                                                          
                                                          
                                                                            digitalWrite(redledPin,HIGH);
                                                                            
                                                                            delay(100);
                                                                            
                                                                            digitalWrite(redledPin,LOW);
                                                                            
                                                                            
                                                          }//endofif codechecker
                        
                                     }//endofif programstate
                       
                        
                      
                       }  //end of if ringstate

}//endof loop









///////////////////functions///////////////



// Listen to Ring
//Listens to the incoming Ringing Pattern and writes it into the Ringing Array

void ListenToRing(){
  
                                       Serial.println("in Listen to ring");    //signal the function


                                       
                                      //define time at start of the listening and current time
                                      
                                      int timestart=millis();                //time at beginning
                                      int now=millis();                      //current time
                                      
                                      
                                      //define 
                                      
                                      boolean Ringstatebefore=false;         //state of input before to check if the state before was ringon/ringoff
                                      boolean change=false;                  //change of input?
                                      int ring =0;                           //ring inkrementer
                                      int Arraystate=0;                      //ON/Off in/de-krementer




	                            //fill Ringingarray


                                      for(int i=0;i<2*NumberofRings;i++){
                                	                               
                                  	                                  RingArray[i]=0;	                     	
                                		                                            
                                	                              }




	    //as long as the time between two rings is shorter than ringfinished and the amount of rings is smaller than maxrings
            //read the pattern

                	        do{
                  
                  
                		               RingState=digitalRead(RingPin);//read the inputstate
                		
                		
                		                
                                                //check if there was a change from ring to no ring or from no ring to ring
       
                                                  if(RingState==Ringstatebefore){
                                    
                                                                                change=false;
                                                          
                                                  }  else {
                                                    
                                                                                change=true;
                                                          
                                                  }//and check if it changed
                		
                
                
                
                                      		//signal for the eyes
                                      
                                      		if(RingState && (change)){//if it changed from off to on signal on
                                        
                                                                          digitalWrite(ledPin, HIGH);
                                                          
                                                }
                                                
                                                
                                                
                                                      
                                      		if((RingState==false) && (change)){//if it changed from on to off
                                      
                                                                                  digitalWrite(ledPin, LOW);
                                                                                  
                                                }
                                                
                                                
                                                
                                                
                                                
                
                                    		//if the signal changed
                                    		if(change){
                                      
                                        			now=millis();//read the time
                                        
                                                                //if it was on , write into the ringing amount collumn of the array 1
                                                                //if it was off, write into the time between collumn of the array 0
                                        
                                        			
                                                                
                                                                
                                                                
                                        			
                                        			RingArray[ring]=(now-timestart);//and fill the entry 
                                        
                                        			Ringstatebefore=RingState;//reset the Beforerringstate
                                        
                                        			timestart=now;		//reset the time
                                        
                                        
                                        
                                                           
                                                               //if it is was ringed signal to computer and increase the ring
                                                               
                                                               
                                                             if(Ringstatebefore==false){
                                                              
                                                                	        	Serial.println(ring);
                                                                
                                                                                        ring++;                //increase ring
                                        		    }//endofif Ringstatebefore
                                    
                                    
                                    
                                    		}//endif change
                
                
                		                  now=millis(); //reset the time
                
                                                  delay(10);    // wait for the signal to fade
                
                
                	} while  (((now-timestart)<ringfinished) && (ring<NumberofRings*2));//end do while time to ring again



}//endof ListenToRing


// function MapArray
//Maps RingArray from 0 to 100, 100 beeing the sum of all


void MapArray(){
  
                                int Sum=0;
                                
                                
                                //calculate cumulated time
                                
                               
                               for(int i=0;i<NumberofRings*2;i++){
                                                                   
                                                                   Sum=Sum+RingArray[i];
                                                                   
                               } 
                                                                
                                
                                
                                
                                //Map Pattern fom 0  to 100
                                
                                for(int i=0;i<NumberofRings*2;i++){
                                                                   
                                                                    RingArray[i]=int(float(RingArray[i])*100/float(Sum));
                                                                    
                                }
                                

}//endof MapArray



//function ChangeMasterArray
// write the current array into the master array


void ChangeMasterArray(){
                         
 
                          Serial.println("ChangemasterArray");
                          
                          
                          for(int i=0;i<NumberofRings*2;i++){
                                                                                 
                                                          MasterArray[i]=RingArray[i];
                                                
                          } 
                          
  
  
}//endof ChangeMAsterArray





//function CheckSecretCode checks if the ringing pattern matches the secret code

boolean CheckSecretCode(int RingArray[],int MasterArray[]){
  
                                        Serial.println("Codecheck");  
                                        
                                        boolean checkthecode=false;                                      
                                        int distance=0;
                                        int distancearray[2*NumberofRings];//to check at the computer     
                                        for(int i=0;i<NumberofRings*2;i++){
                                                                           distancearray[i]=0;
                                        }                                  
                                         
                                        // check the whole array and calculate the difference to the masterarray
                                        
                                        
                                        for(int i=0;i<NumberofRings*2;i++){
                                                                        
                                                  
                                                                                             distance=abs(RingArray[i]-MasterArray[i]);    //calculate distance
                                                                                             distancearray[i]=distance;
                                                                                                                                              
                                                   
                                                                                             if(distance<=Sensibility){
                                                                                                                        checkthecode=true; //if entry is right checkthecode is true
                                                                                             } else {
                                                                                                                        return false;//if only one entry is wrong return false
                                                                                             }//endifelse
                                                       
                                                                         
                                        } 
                                         
                                         Printpattern(distancearray);  //to check at the computer
                                          
                                          return checkthecode;
  
}//endof Check Secret Code





//function Blink blinks the Pattern that is send to it

void Blink(int Blinkpattern [NumberofRings*2]){
  
  
                          int delayI=0;
                          
                          
                          for(int i=0;i<NumberofRings*2;i++){
                          
                                                              //Off
                                                            digitalWrite(ledPin,LOW);
                                                            delayI=Blinkpattern[i]*20;
                                                            delay(delayI);
                                                             
                                                             i++;
                                                             
                                                             //On
                                                            digitalWrite(ledPin,HIGH);
                                                            delayI=Blinkpattern[i]*20;
                                                            delay(delayI);
                           
                           }//endoffor
                           
                          //Off again
                         digitalWrite(ledPin,LOW);
                         //wait short time
                         delay(50);


}//endof Blinkpattern





//function Printpattern Prints the Pattern to the Serial Port

void Printpattern(int Blinkpattern [NumberofRings*2]){
  
  
                                         for(int i=0;i<NumberofRings*2;i++){
                                                                          
                                                                           Serial.print(Blinkpattern[i]);
                                                                           Serial.print("  ");
                                                                            
                                         } 
                                  

}//endof PrintPattern

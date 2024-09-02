

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

#define buttonUp 2
#define buttonDown 3
#define buttonSelect 4
#define buttonCancel 5
#define LiquidCrystal_I2C_ADDRESS 0x27


#define MOVECURSOR 1  // constants for indicating whether cursor should be redrawn
#define MOVELIST 2  // constants for indicating whether cursor should be redrawn
byte totalRows = 4;  // total rows of LCD
byte totalCols = 20;  // total columns of LCD
unsigned long timeoutTime = 0;  // this is set and compared to millis to see when the user last did something.
const int menuTimeout = 10000; // time to timeout in a menu when user doesn't do anything.

unsigned long lastButtonPressed; // this is when the last button was pressed. It's used to debounce.
const int debounceTime = 200; //this is the debounce and hold delay. Otherwise, you will FLY 
                                         //through the menu by touching the button. 

LiquidCrystal_I2C lcd(LiquidCrystal_I2C_ADDRESS,2,1,0,4,5,6,7);  // Set the LCD I2C address

void setup(){
  Wire.begin();
  // Serial.begin(9600);
  // Serial.println(""); Serial.println("Water System by Asaf Goldis"); Serial.println("");
  
  lcd.begin (totalCols,totalRows); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonCancel, INPUT_PULLUP);
}


int read_buttons() {  // you may need to swap "void" with "int" or "byte"
  byte returndata = 0;
  int buttonState; // this might not be the correct declaration.
  // remember to declare what buttonUp, buttonDown, buttonSelect, buttonCancel are
  if ( (lastButtonPressed + debounceTime) < millis() ) {  // see if it's time to check the buttons again
    buttonState = digitalRead(buttonUp);
    if (buttonState == LOW){
      returndata = returndata + 1;
      lastButtonPressed = millis();
    }
  
    buttonState = digitalRead(buttonDown);
    if (buttonState == LOW){
      returndata = returndata + 2;
      lastButtonPressed = millis();
    }
  
    buttonState = digitalRead(buttonSelect);
    if (buttonState == LOW){
      returndata = returndata + 4;
      lastButtonPressed = millis();
    }
  
    buttonState = digitalRead(buttonCancel);
    if (buttonState == LOW) {
      returndata = returndata + 8;
      lastButtonPressed = millis();
    }
  }
 return returndata; // this spits back to the function that calls it the variable returndata.
}


void loop() {
  //Serial.println(digitalRead(buttonSelect));
  
  byte topItemDisplayed = 0;  // stores menu item displayed at top of LCD screen
  byte cursorPosition = 0;  // where cursor is on screen, from 0 --> totalRows. 

  // redraw = 0  - don't redraw
  // redraw = 1 - redraw cursor
  // redraw = 2 - redraw list
  byte redraw = MOVELIST;  // triggers whether menu is redrawn after cursor move.
  byte i=0; // temp variable for loops.
  byte totalMenuItems = 0;  //a while loop below will set this to the # of menu items.

// Put the menu items here. Remember, the first item will have a 'position' of 0.
  char* menuItems[]={ "Set Timer 1", 
                      "Set Timer 2", 
                      "Set Probe 1 Temp",
                      "Set Probe 2 Temp", 
                      "Probe 1 Controller",
                      "Probe 2 Controller",
                      "Advanced Settings",
                      "",
                      };

  while (menuItems[totalMenuItems] != "") {
    totalMenuItems++;  // count how many items are in list.
  }
  totalMenuItems--;  //subtract 1 so we know total items in array.

  lcd.clear();  // clear the screen so we can paint the menu.

  boolean stillSelecting = true;  // set because user is still selecting.

  timeoutTime = millis() + menuTimeout; // set initial timeout limit. 
  Serial.println("111");
  while (stillSelecting == true) {  //  // loop while waiting for user to select.

    /*
    IF YOU WANT OTHER CODE GOING ON IN THE BACKGROUND
    WHILE WAITING FOR THE USER TO DO SOMETHING, PUT IT HERE
    */
    
    /*
    my code uses a rotary encoder for input. 
    You should obviously change the code to meet your needs.
    For a button, you could do something like this, but note that
    it does not have ANY debouncing and will scroll for as long as
    the button is being pushed. This is not a button tutorial, 
    so you should look elsewhere on how to implement that. Just 
    remember that ALL of the code between the corresponding 'case'
    and 'break' should be moved to each button push routine.
    
    
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {     
      AND THEN PUT THE CORRESPONDING CODE
      FROM BELOW HERE
  } 
  */

  
  switch( read_buttons() ) {  // analyze encoder response. Default is 0.
    case 1:  
      timeoutTime = millis() + menuTimeout;  // reset timeout timer
      //  if cursor is at top and menu is NOT at top
      //  move menu up one.
      if(cursorPosition == 0 && topItemDisplayed > 0) {  //  Cursor is at top of LCD, and there are higher menu items still to be displayed.
      topItemDisplayed--;  // move top menu item displayed up one. 
      redraw = MOVELIST;  // redraw the entire menu
      }
      
      // if cursor not at top, move it up one.
      if(cursorPosition>0) {
      cursorPosition--;  // move cursor up one.
      redraw = MOVECURSOR;  // redraw just cursor.
      }
    break;
    
    case 2:    
      timeoutTime = millis()+menuTimeout;  // reset timeout timer
      // this sees if there are menu items below the bottom of the LCD screen & sees if cursor is at bottom of LCD 
      if((topItemDisplayed + (totalRows-1)) < totalMenuItems && cursorPosition == (totalRows-1)) {
      topItemDisplayed++;  // move menu down one
      redraw = MOVELIST;  // redraw entire menu
      }
      if(cursorPosition<(totalRows-1)) {  // cursor is not at bottom of LCD, so move it down one.
      cursorPosition++;  // move cursor down one
      redraw = MOVECURSOR;  // redraw just cursor.
      }
    break;
    
    case 4:
      timeoutTime = millis()+menuTimeout;  // reset timeout timer
      switch(topItemDisplayed + cursorPosition) { // adding these values together = where on menuItems cursor is.
        //  put code to be run when specific item is selected in place of the Serial.print filler.
        // the Serial.print code can be removed, but DO NOT change the case & break structure. 
        // (Obviously, you should have as many case instances as you do menu items.)
        case 0:  // menu item 1 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
          // there is no menuSubMenu1() function. BUT, to have nested menus,
          // copy this function(i.e. all of basicMenu) into a new function named 
          // whatever you want for your sub menu items and repeat.
          //        menuSubMenu1();
        break;
      
        case 1:  // menu item 2 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
        
        case 2:  // menu item 3 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
        
        case 3:  // menu item 4 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
      
        case 4:  // menu item 5 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
        
        case 5:  // menu item 6 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
        
        case 6:  // menu item 7 selected
          Serial.print("Menu item ");
          Serial.print(topItemDisplayed + cursorPosition);
          Serial.print(" selected - ");
          Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
      
      // add as many "case #:" as items you have. You could put 
      //  line separators in menuList and leave out the 
      //  corresponding case, which would mean that nothing
      // would be triggered when user selected the line separator.  
      }
      break;
    
    case 8:  // encoder button was pushed for long time. This corresponds to "Back" or "Cancel" being pushed.
      stillSelecting = false;
      Serial.println("Button held for a long time");
    break;
  }

  switch(redraw) {  //  checks if menu should be redrawn at all.
    case MOVECURSOR:  // Only the cursor needs to be moved.
      redraw = false;  // reset flag.
      if (cursorPosition > totalMenuItems) // keeps cursor from moving beyond menu items.
        cursorPosition = totalMenuItems;
      for(i = 0; i < (totalRows); i++) { // loop through all of the lines on the LCD
        lcd.setCursor(0,i);
        lcd.print(" ");                      // and erase the previously displayed cursor
        lcd.setCursor((totalCols-1), i);
        lcd.print(" ");
      }
      lcd.setCursor(0,cursorPosition);      // go to LCD line where new cursor should be & display it.
      lcd.print(">");
      lcd.setCursor((totalCols-1), cursorPosition);
      lcd.print("<");
    break;  // MOVECURSOR break.
    
    case MOVELIST:  // the entire menu needs to be redrawn
      redraw=MOVECURSOR;  // redraw cursor after clearing LCD and printing menu.
      lcd.clear(); // clear screen so it can be repainted.
      if(totalMenuItems>((totalRows-1))) {  // if there are more menu items than LCD rows, then cycle through menu items.
        for (i = 0; i < (totalRows); i++) {
          lcd.setCursor(1,i);
          lcd.print(menuItems[topItemDisplayed + i]);
        }
      }
      else {  // if menu has less items than LCD rows, display all available menu items.
        for (i = 0; i < totalMenuItems+1; i++) {
          lcd.setCursor(1,i);
          lcd.print(menuItems[topItemDisplayed + i]);
        }
      }
    break;  // MOVELIST break
  }
  
     if (timeoutTime<millis()) {  // user hasn't done anything in awhile
       stillSelecting = false;  // tell loop to bail out.
       /*
       in my main code, I had a function that
        displayed a default screen on the LCD, so
        I would put that function here, and it would
        bail out to the default screen.
        defaultScreen();
        */
     }
   }

 
}




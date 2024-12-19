#include "ps2KeyboardController.h"
PS2KeyAdvanced keyboardPs2;

void ps2KeyboardController::begin()
{
    // Configure the keyboard library
    keyboardPs2.begin(PS2_DATA, PS2_CLK);
    Serial.print("PS2 keyboard started");
}

ConsoleKeyPress ps2KeyboardController::getKey()
{
    ConsoleKeyPress kp(None, _c);
    if (keyboardPs2.available())
    {
        // read the next key
        if ((_c = keyboardPs2.read()))
        {
            
            // check for some of the special keys
            Serial.print("Processing PS2 key 0x");
            Serial.println(_c, HEX);

            
            kp.action = (_c & 0x8000) ? KeyUp : KeyDown;
            kp.isShiftPressed = (_c & 0x4000);
            kp.isCtrlPressed = (_c & 0x2000);
            kp.isCaps = (_c & 0x1000);
            kp.isAltPressed = (_c & 0x800) |  (_c & 0x400);
            kp.isFunctionKey = (_c & 0x100);

            //if(!) return kp; //if not key up, wait
            _mode = 2;
            _c &= 0xFF;
            if(kp.isFunctionKey){
                kp.type = Cursor;
                kp.keyCode = _c;
                _lastKey = new ConsoleKeyPress(kp);
                return kp;  
            } 
            if(kp.isCtrlPressed || kp.isAltPressed){
                kp.type = ConsoleKeyType::Control;
                kp.keyCode = _c;
                _lastKey = new ConsoleKeyPress(kp);
                return kp;
            }
            
            _mode = 0;
        

            /* Check for strings or single character to display */
            /* Function or similar key */
            if (_c != PS2_KEY_EUROPE2 && (_c < PS2_KEY_SPACE || _c >= PS2_KEY_F1) && _c != PS2_KEY_TILDE)
            { // Non printable sort which ones we can print
                Serial.println("Nonprint chars");
                kp.type = ConsoleKeyType::Control;
                for (size_t idx = 0; idx < sizeof(codes); idx++)
#if defined(PS2_REQUIRES_PROGMEM)
                    if (c == pgm_read_byte(codes + idx))
#else
                    if (_c == codes[idx])
#endif
                    { /* String outputs */
                        _mode = 1;
#if defined(PS2_REQUIRES_PROGMEM)
                        _c = pgm_read_byte(sizes + idx);
#else
                        //_c = sizes[idx];
#endif
#if defined(PS2_REQUIRES_PROGMEM)
                        strcpy_P(buffer, (char *)pgm_read_word(&(keys[idx])));
                        lcd.print(buffer);

#else
                        {
                            // function keys, del
                            // if(editor.IsConsoleRunning()){
                            switch (_c)
                            {
                            case PS2_KEY_F2:
                            case PS2_KEY_F3:
                            Serial.print("Changing color .. ");
                                kp.type = ColorChange;
                                kp.keyCode = _c;
                                _lastKey = new ConsoleKeyPress(kp);
                                return kp;  
                            case PS2_KEY_F4:
                                kp.type = Exit;
                                kp.keyCode = _c;
                                _lastKey = new ConsoleKeyPress(kp);
                                return kp;  
                            default:
                                // Serial.print("Received unmapped ps2 key: 0x"); Serial.println(_c, HEX);
                                break;
                            }
                            //}
                        }
                        // Serial.print(keys[ idx ]);
                        // lcd.print(  );
#endif
                        // check_cursor( );
                        break;
                    }
                /* if not found a string ignore key cant do anything */
            }
            else
            { /* Supported key */
                if (_c <= 127 || _c > 0)
                {
                    kp.type = ASCII;                   
                    if(
                        ((!kp.isShiftPressed && !kp.isCaps) || (kp.isShiftPressed && kp.isCaps))
                        && (_c > 0x40 && _c <= 0x5A)
                    ){
                        _c = _c + 0x20; //lowercase letters
                    }
                    else if(kp.isShiftPressed){
                        switch (_c)
                        {
                        // case 0x29:
                        //     _c = PS2_KEY_UNDERSCORE;
                        //     break;
                        case PS2_KEY_SINGLE_QUOTE:
                            _c = PS2_KEY_DOUBLE_QUOTE;
                            break;
                        case PS2_KEY_DIV:
                            _c = '?';
                            break;
                        case PS2_KEY_COMMA:
                            _c = '<';
                            break;
                        case PS2_KEY_DOT:
                            _c = '>';
                            break;
                        case 0x2D:
                            _c = PS2_KEY_UNDERSCORE;
                            break;
                        case 0x30:
                            _c = PS2_KEY_PARAN_CLOSE;
                            break;
                        case 0x31:
                            _c = PS2_KEY_EXCLAMATION;
                            break;
                        case 0x32:
                            _c = PS2_KEY_AT;
                            break;
                        case 0x33:
                            _c = PS2_KEY_HASH;
                            break;
                        case 0x34:
                            _c = PS2_KEY_DOLLAR;
                            break;
                        case 0x35:
                            _c = PS2_KEY_PERCENT;
                            break;
                        case 0x36:
                            _c = PS2_KEY_CARROT;
                            break;
                        case 0x37:
                            _c = PS2_KEY_AMPERSAND;
                            break;
                        case 0x38:
                            _c = PS2_KEY_KP_TIMES;
                            break;
                        case 0x39:
                            _c = PS2_KEY_PARAN_OPEN;
                            break;
                        case 0x3D:
                            _c = PS2_KEY_KP_PLUS;
                            break;
                        case 0x60:
                            _c = PS2_KEY_TILDE;
                            break;
                        
                        case 0x5B:
                            _c = PS2_KEY_OPEN_CU;
                            break;
                        case 0x5C:
                            _c = PS2_KEY_PIPE;
                            break;
                        case PS2_KEY_CLOSE_SQ:
                            _c = PS2_KEY_CLOSE_CU;
                            break;
                        case PS2_KEY_SEMI:
                            _c = PS2_KEY_COLON;
                            break;
                        
                        
                        // case 0x30:
                        //     _c = PS2_KEY_TILDE;
                        //     break;
                        // case 0x30:
                        //     _c = PS2_KEY_TILDE;
                        //     break;    
                        default:
                            break;
                        }
                        
                    }
                    kp.keyCode = _c;
                    _lastKey = new ConsoleKeyPress(kp);
                    return kp;  
                }
            }
        }
    }
    return kp;
}

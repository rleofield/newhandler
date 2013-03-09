// newhandler.cpp : Defines the entry point for the console application.
//


#include <string>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>


using namespace std;


// Demos zu:
// http://www.lug-ottobrunn.de/wiki/Operator_New_in_CPP

/*! In C++ gibt es 3 Varianten des 'operators new()',
    die sich im Verhalten unterscheiden, wenn kein Speicher mehr angefordert werden kann.

  1. Es wird eine Exception 'bad_alloc' geworfen
     new() kehrt nicht zur�ck, es wird kein NULL zur�ckgegeben

  2. Es wird eine Funktion aufgerufen, die mit 'set_new_handler()' gesetzt werden kann
     Jeder Programmabschnitt kann einen eigen New Handler haben.
     new() gibt in diesem Fall auch kein NULL zur�ck und l�st auch keine Exception aus.

  3. Mit dem Keywort '(nothrow)' kann new() ein Verhalten aufgezwungen werden,
     wie das in C mit malloc() der Fall ist.
     Es wird NULL zur�ckgegeben.

     Bitte beachten:
			NULL ist in C++ anders definiert als in C!
			C++ NULL ist (int)0
			C   NULL ist (void*)0
			In C ist NULL ein Pointer und in C++ ein 'int'.


   Variante 1 ist der das Default Verhalten von new().

   Varianten 1 und 2 k�nnen mit C++ Mitteln �berladen werden, Variante 3 nicht.

   Variante 3 ist nicht zu empfehlen, weil die Veranwortung der Kontrolle
   �ber den Speicher dem Entwickler �bertragen muss.
   Vergessene Pr�fungen auf NULL sind eine sehr h�ufige Ursache f�r unvorhergesehene
   Programmabst�rze, die mit dem Einsatz der Mittel von C++
   der Vergangenheit angeh�ren sollten.

   C++ erlaubt mit Variante 1. einen 'Single Point of Failure' zu erstellen,
   an dem die Behandlung von Speichermangel definiert erfolgen kann.

www.cplusplus.com/reference/std/new/operator new/

  */



class BadConversion : public std::runtime_error
{
public:
    BadConversion( const std::string& msg )
        : std::runtime_error( msg ) { }
};


template<class T >
inline std::string toString( T val  )
{
    std::ostringstream o;
    if ( !( o << val ) )
        throw BadConversion( std::string( "toString(" ) + typeid( val ).name() + ")" );
    return o.str();
}

void DemoNewHandler(){
  cout << "new handler " << endl;
  // Was nun?
  // Anderen Speicher freigeben und new() wiederholen?
  // Oder:
  // Logzeile schreiben, wenn noch m�glich und mit abort() das Programm verlassen?

}

namespace {
    const size_t count_ = 16;
		// unter 32 bit anpassen
    const unsigned long long bufsize = 1024L*1024L*1024L;
}

/*! Demostration, dass 'operator new()'im Fehlerfall den New Handler aufruft.
*/
void doNewHandlerDemo( void ) {

   void (*oldNewHandler)() = set_new_handler(DemoNewHandler);
   char * p = 0;
   for ( size_t i=0; i < count_; i++){
        try{
          p = new char[bufsize];
        }
        catch( std::bad_alloc &b ){
           // wird nie erreicht
          cout << "test: "  << b.what () << endl;
          exit( 0 );
        }

    if( p == 0 ){
        // wird nie erreicht
        cout << "test" << endl;
    }
   }
   // zur�cksetzen des fr�heren New handlers
   // Hier bietet sich RAII an, da eine Methode nicht
   //   immer an einer vom Entwickler vorhersehbaren Stelle verlassen wird.
   set_new_handler(oldNewHandler);
   return ;
}

/*! Demostration, dass 'operator new()'im Fehlerfall eine Exception wirft,
    wenn kein New Handler gesetzt wurde.
  */
void doNewExceptionDemo() {

   for ( size_t i=0; i < count_; i++){
        try{
          char * p = new char[bufsize];
        }
        catch( std::bad_alloc &b ){
          cout << "korrekt, Exception: "  << b.what () << endl;
          // Was nun?
          // Anderen Speicher freigeben und new() wiederholen? Wie?
          // Oder:
          // Logzeile schreiben, wenn noch m�glich und mit abort() das Programm verlassen
          abort();
        }
    }
   return ;
}


/*! Demostration, dass 'operator new()' niemals NULL liefert und das Prgramm
    hoffnungslos abst�rzt, wenn die Excption nicht behandelt wird.
  */
void doNewNullFailDemo() {
   for ( size_t i =0; i < count_; i++){
      cout << "count: "  << toString(i) << endl;

      // alloziere etwas RAM
      char * p = new char[bufsize];

      // Test auf die R�ckgabe von NULL, in C++ nicht m�glich
      if( p == NULL ){
        // das Programm kommt nie an diese Stelle
        cout << "Test Ende bei NULL: "  << toString(i) << endl;
        exit( 0 );
      }
      // Abbruch mit dem Text, Linux GCC:
      // "The inferior stopped because it received a signal from the Operating System."
      //
      // Signal name :
      // SIGABRT
      // Signal meaning :
      // Aborted

			// MS VC2010
			// Unhandled exception at 0x7708b9bc in newhandler.exe: Microsoft C++ exception: std::bad_alloc at memory location 0x003df6a8..
			// stop in mlock.c - Multi-thread locking routines
			// Zeile 371
			// d.h. intern in Win, nicht an der Stelle, an der Fehler auftrat
			

   }
   return ;
}

/*! Demonstration, dass 'operator new() (nothrow)' ein Verhalten wie in C erzwungen werden kann
  d.h. Es wird NULL zur�ckgegeben.
  */
void doNewNullFailErzwungenes_C_VerhaltenDemo() {
   for ( size_t i =0; i < count_; i++){
      cout << "count: "  << toString(i) << endl;

      // alloziere etwas RAM, mit (nothrow) in new()
      char * p = new (nothrow) char[bufsize];

      // Test auf die R�ckgabe von NULL, in C++ nicht m�glich
      if( p == NULL ){
        // das Programm kommt an diese Stelle, weil die Exception unterdr�ckt wurde
        cout << "Test Ende bei NULL: "  << toString(i) << endl;
        exit( 0 );
      }

   }
   return ;
}


// bitte die Methoden zum Testen ein-/auskommentieren
int main( void ){
   //doNewNullFailErzwungenes_C_VerhaltenDemo();
   //doNewNullFailDemo();
  // doNewExceptionDemo();
   doNewHandlerDemo();
   return 0;
}



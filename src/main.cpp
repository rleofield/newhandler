
#include <string>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>


using namespace std;


/*! In C++ gibt es 3 Varianten des 'operators new()',
    die sich im Verhalten unterscheiden, wenn kein Speicher mehr angefordert werden kann.

  1. Es wird eine Exception 'bad_alloc' geworfen
     new() gibt in diesem Fall nie NULL zurück.

  2. Es wird eine Funktion aufgerufen, die mit 'set_new_handler()' gesetzt werden kann
     Jeder Programmabschnitt kann einen eigen New Handler haben.
     new() gibt in diesem Fall nie NULL zurück und löst auch keine Exception aus.

  3. Mit dem Keywort '(nothrow)' kann new() ein Verhalten aufgezwungen werden,
     wie das in C mit malloc() der Fall ist.
     Es wird 0 zurückgegeben.
     Bitte beachten:
     NULL ist in C++ anders definiert als in C!
     C++ NULL ist (int)0
     C   NULL ist (void*)0
     In C ist NULL ein Pointer und in C++ ein 'int'.


   Variante 1 ist der das Default Verhalten von new().

   Varianten 1 und 2 können mit C++ Mitteln überladen werden, Variante 3 nicht.

   Variante 3 ist nicht zu empfehlen, weil die Veranwortung der Kontrolle
   über den Speicher dem Entwickler übertragen muss.
   Vergessene Prüfungen auf NULL sind eine sehr häufige Ursache für unvorhergesehene
   Programmabstürze, die mit dem Einsatz der Mittel von C++
   der Vergangenheit angehören sollten.

   C++ erlaubt mit Variante 1. einen 'Single Point of Failure' zu erstellen,
   an dem die Behandlung von Speichermangel definiert erfolgen kann.

   siehe: www.cplusplus.com/reference/std/new/operator new/

  */



class BadConversion : public std::runtime_error {
public:
   BadConversion( const std::string& msg )
      : std::runtime_error( msg ) { }
};


template<class T >
inline std::string toString( T val ) {
   std::ostringstream o;

   if( !( o << val ) ) {
      throw BadConversion( std::string( "toString(" ) + typeid( val ).name() + ")" );
   }

   return o.str();
}

void DemoNewHandler() {
   cout << "new handler " << endl;
   // Was nun?
   // Anderen Speicher freigeben und new() wiederholen?
   // Oder:
   // Logzeile schreiben, wenn noch möglich und mit abort() das Programm verlassen?

}

namespace local {
   const size_t count = 16;
   const size_t bufsize = 32L * 1024 * 1024 * 1024;
}

/*! Demostration, dass 'operator new()'im Fehlerfall den New Handler aufruft.
*/
void doNewHandlerDemo( void ) {

   void ( *oldNewHandler )() = set_new_handler( DemoNewHandler );
   char* p = 0;

   for( size_t i = 0; i < local::count; i++ ) {
      try {
         p = new char[local::bufsize];
      } catch( std::bad_alloc& b ) {
         // wird nie erreicht
         cout << "test: "  << b.what() << endl;
         exit( 0 );
      }

      if( p == 0 ) {
         // wird nie erreicht
         cout << "test" << endl;
      }
   }

   // zurücksetzen des früheren New handlers
   // Hier bietet sich RAII an, da eine Methode nicht
   //   immer an einer vom Entwickler vorhersehbaren Stelle verlassen wird.
   set_new_handler( oldNewHandler );
   return ;
}

char* p ;

/*! Demostration, dass 'operator new()'im Fehlerfall eine Exception wirft,
    wenn kein New Handler gesetzt wurde.
  */
void doNewExceptionDemo( void ) {

   for( size_t i = 0; i < local::count; i++ ) {
      try {
         p = new char[local::bufsize];
      } catch( std::bad_alloc& b ) {
         cout << "korrekt, Exception: "  << b.what() << endl;
         // Was nun?
         // Anderen Speicher freigeben und new() wiederholen? Wie?
         // Oder:
         // Logzeile schreiben, wenn noch moeglich und mit abort() das Programm verlassen
         abort();
      }
   }

   return ;
}


/*! Demostration, dass 'operator new()' niemals NULL liefert und das Prgramm
    hoffnungslos abstürzt, wenn die Excption nicht behandelt wird.
  */
void doNewNullFailDemo( void ) {
   for( size_t i = 0; i < local::count; i++ ) {
      cout << "count: "  << toString( i ) << endl;

      // alloziere etwas RAM
      char* p1 = new char[local::bufsize];

      // Test auf die Rückgabe von NULL, in C++ Unsinn
      if( p1 == NULL ) {
         // das Programm kommt nie an diese Stelle !!!!!!!!!!!!!!!!!!!!!!!!!
         cout << "Test Ende bei NULL: "  << toString( i ) << endl;
         exit( 0 );
      }

      // fataler Absturz, im Debugger mit dem Text für Linux GCC:
      // "The inferior stopped because it received a signal from the Operating System."
      //

   }

   return ;
}

/*! Demonstration, dass 'operator new() (nothrow)' ein Verhalten wie in C erzwungen werden kann
  d.h. Es wird NULL zurückgegeben.
  */
void doNewNullFailErzwungenes_C_VerhaltenDemo( void ) {
   for( size_t i = 0; i < local::count; i++ ) {
      cout << "count: "  << toString( i ) << endl;

      // alloziere etwas RAM, mit (nothrow) in new()
      char* p2 = new( nothrow ) char[local::bufsize];

      // Test auf die Rückgabe von NULL, in C++ nicht möglich
      if( p2 == NULL ) {
         // das Programm kommt an diese Stelle, weil die Exception unterdrückt wurde
         cout << "Test Ende bei NULL: "  << toString( i ) << endl;
         exit( 0 );
      }

   }

   return ;
}


// bitte die Methoden zum Testen ein-/auskommentieren
int main( void ) {
   //doNewNullFailErzwungenes_C_VerhaltenDemo(); // Bitte nie so arbeiten!
   //doNewNullFailDemo();  // wir sind hier nicht in C, solcher Code führt zu Programmabstürzen!
   //doNewExceptionDemo(); // korrekt C++
   doNewHandlerDemo();     // korrekt C++
   return 0;
}


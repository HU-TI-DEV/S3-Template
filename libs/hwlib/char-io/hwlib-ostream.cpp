#include "hwlib.hpp"
#include "hwlib-ostream.hpp"

// this file contains Doxygen lines
/// @file

namespace hwlib {

// ==========================================================================
//
// ostream format manipulators
//
// ==========================================================================

  
// ==========================================================================
//
// formatted ostream
//
// ==========================================================================
  
/// formatted character output interface
///
/// This class is an std::ostream work-alike for small embedded systems.
/// Most formatting features of std::ostream are supported.
/// Floating point values are not supported.
/// 
/// This class is abstract: a concrete subclass 
/// must implement putc() and flush().   
       

      
   constexpr ostream::ostream(): 
      field_width( 0 ), 
      numerical_radix( 10 ),
      fill_char( ' ' ), 
      hex_base( 'A' ),
      align_right( true ), 
      show_pos( false ),
      bool_alpha( false ),
      show_base( false )
   {}
 
       
   /// char output operator 
   ostream & operator<< ( char c ){ 
      putc( c ); 
      return *this; 
   }
      
      
   /// \cond INTERNAL
   ostream & operator<< ( ostream & stream, const _flush x ){
      stream.flush();
      return stream;
   }
   /// \endcond      
      
      
   // =======================================================================
   //
   // manipulators
   //
   // =======================================================================
  
   /// return the current field width
   uint_fast16_t ostream::width( void ) const { return field_width; }
      
   /// set the field width, return the old field width
   uint_fast16_t ostream::width( uint_fast16_t x ) { 
      auto temp = field_width; 
      field_width = x; 
      return temp;
   }
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const setw & x ){
      stream.width( x.x );
      return stream;
   }
   /// \endcond 
      
   /// return the numerical radix       
   uint_fast16_t ostream::base( void ) const { return numerical_radix; }
      
   /// set the numerical radix, return the old numerical radix  
   uint_fast16_t ostream::base( uint_fast16_t x ) { 
      auto temp = numerical_radix;
      numerical_radix = x; 
      return temp;
   }
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const _setbase & x ){
      stream.numerical_radix = x.x;
      return stream;
   }
   /// \endcond 
      
   /// return the current showpos setting        
   bool ostream::showpos( void ) const { return show_pos; }
      
   /// set the showpos setting, return the old showpos setting 
   bool ostream::showpos( bool x ) { 
      bool temp = show_pos;
      show_pos = x;
      return temp;
   }   
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const _showpos & x ){
      stream.show_pos = x.x;
      return stream;
   }
   /// \endcond 
      
   /// return the current showbase setting        e
   bool ostream::showbase( void ) const { return show_base; }
      
   /// set the showbase setting, return the old showbase setting     
   bool ostream::showbase( bool x ){ 
      bool temp = show_base;
      show_base = x; 
      return temp;
   }
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const _showbase & x ){
      stream.show_base = x.x;
      return stream;
   }      
   /// \endcond 
      
   /// return the current boolalpha setting        
   bool ostream::boolalpha( void ) const { return bool_alpha; }
      
   /// set the noboolalpha setting, return the old noboolalpha setting       
   bool ostream::boolalpha( bool x ) { 
      bool temp = bool_alpha;
      bool_alpha = x; 
      return temp;
   }
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const _boolalpha & x ){
      stream.bool_alpha = x.x;
      return stream;
   }
   /// \endcond 
   
   /// set the fill char setting, return the old fill char setting
   char ostream::fill( char x ){ 
      char temp = fill_char;
      fill_char = x; 
      return temp;
   }
      
   /// \cond INTERNAL      
   ostream & operator<< ( ostream & stream, const setfill x ){
      stream.fill_char = x.x;
      return stream;
   }      
   /// \endcond 
      
   /// \cond INTERNAL      
   void ostream::right( void ){ align_right = true; }      
   ostream & operator<< ( ostream & stream, const _right x ){
      stream.align_right = true;
      return stream;
   }
   /// \endcond 
      
   /// \cond INTERNAL      
   void ostream::left( void ){ align_right = false; }     
   ostream & operator<< ( ostream & stream, const _left x ){
      stream.align_right = false;
      return stream;
   }
   /// \endcond 
      
   // bool must_align_right( void ){ return align_right; }
      
      
   // =======================================================================
   //
   // print bool and string types
   //
   // =======================================================================
      
   /// output operator for bool
   ostream & operator<< ( ostream & stream, bool x ){
      if( stream.boolalpha()){
         stream << ( x ? "true" : "false" );
      } else {
         stream << ( x ? "1" : "0" );
      }
      return stream;   
   }      
  
   /// output operator for const char pointer (literal string)
   ostream & operator<< ( ostream & stream, const char *s ){
      if( stream.align_right ){
         stream.filler( static_cast< int_fast16_t >( 
		    stream.width()) - strlen( s )); 
      }       
      for( const char *p = s; *p != '\0'; p++ ){
         stream << *p;
      }
      if( ! stream.align_right ){
        stream.filler( static_cast< int_fast16_t >( 
		   stream.width()) - strlen( s )); 
      }  
      stream.width( 0 );
      return stream;
   }    
  
  
   // =======================================================================
   //
   // print integers
   //
   // =======================================================================
      
   /// output operator for integer      
   ostream & operator<< ( ostream & stream, int x ){
      reverse s;
         
      bool minus = ( x < 0 );
      if( x < 0 ){ x = -x; }
         
      if( x == 0 ){
         s.add_digit( 0, stream.hex_base );
      }
      while( x > 0 ){
         s.add_digit( x % stream.base(), stream.hex_base );
         x = x / stream.base();
      }
      s.add_prefix( stream );
         
      if( minus ){
         s.add_char( '-' );
      } else if( stream.showpos() ){
         s.add_char( '+' );
      }        
         
      stream << s.content;
      return stream;   
   }
   
   /// output operator for short integer   
   ostream & operator<< ( ostream & stream, short int x ){
      return stream << static_cast< int >( x );
   }
   
   /// output operator for long integer   
   ostream & operator<< ( ostream & stream, long int x ){
      return stream << static_cast< int >( x );
   }
   
   /// output operator for long long integer   
   ostream & operator<< ( ostream & stream, long long int x ){
      reverse s;

      bool minus = ( x < 0 );
      if( x < 0 ){ x = -x; }

      if( x == 0 ){
         s.add_digit( 0, stream.hex_base );
      }
      while( x != 0 ){
         s.add_digit( x % stream.base(), stream.hex_base );
         x = x / stream.base();
      }
      s.add_prefix( stream );

      if( minus ){
         s.add_char( '-' );
      } else if( stream.showpos() ){
         s.add_char( '+' );
      }   
         
      stream << s.content;
      return stream;   
   }
   
   /// output operator for short unsigned integer   
   ostream & operator<< ( ostream & stream, short unsigned int x ){
      return stream << static_cast< long long int >( x );      
   }
   
   /// output operator for unsigned integer   
   ostream & operator<< ( ostream & stream, unsigned int x ){
      return stream << static_cast< long long int >( x );      
   }
   
   /// output operator for unsigned long integer   
   ostream & operator<< ( ostream & stream, unsigned long int x ){
      return stream << static_cast< long long int >( x ); 
   }
 
   /// output operator for unsigned long long integer    
   ostream & operator<< ( ostream & stream, unsigned long long x ){
      return stream << static_cast< long long int >( x ); 
   }
   
   /// output operator for signed char (prints as integer)   
   ostream & operator<< ( ostream & stream, signed char c ){
      return stream << static_cast< int >( c ); 
      //stream.putc( c );
      //return stream;
   }
   
   /// output operator for unsigned char (prints as integer)   
   ostream & operator<< ( ostream & stream, unsigned char c ){
      return stream << static_cast< int >( c ); 
      //stream.putc( c );
      //return stream;
   }
      
}; // class ostream  

}; // namespace hwlib

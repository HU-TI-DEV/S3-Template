// ==========================================================================
//
// File      : hwlib-ostream.hpp
// Part of   : C++ hwlib library for close-to-the-hardware OO programming
// Copyright : wouter@voti.nl 2017-2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================

// included only via hwlib.hpp, hence no multiple-include guard is needed

#pragma once

// this file contains Doxygen lines
/// @file

namespace hwlib {

/// end-of-line constant
constexpr char endl = '\n';
   
/// 0-character constant
constexpr char ends = '\0';

// ==========================================================================
//
// ostream format manipulators
//
// ==========================================================================

/// ostream output field width manipulator
struct setw
{ const uint_fast16_t x;
   constexpr setw( uint_fast16_t x ) : x( x ){}
};
         
struct _setbase {       
   const uint_fast16_t x;
   constexpr _setbase( uint_fast16_t x ) : x( x ){}
};
   
/// set ostream radix to 2
///
/// When this manipulator is written to an ostream the base for
/// printing numbers is set to 2 (binary).
constexpr _setbase bin( 2 );
   
/// set ostream radix to 8
///
/// When this manipulator is written to an ostream the base for
/// printing numbers is set to 8 (octal).
constexpr _setbase oct( 8 );
   
/// set ostream radix to 10
///
/// When this manipulator is written to an ostream the base for
/// printing numbers is set to 10 (decimal).
/// This is the inital situation.
constexpr _setbase dec( 10 );
   
/// set ostream radix to 16
///
/// When this manipulator is written to an ostream the base for
/// printing numbers is set to 16 (hexadecimal).
constexpr _setbase hex( 16 );
   
struct _showpos {      
   const bool x; 
      constexpr _showpos( bool x ) : x( x ){}
}; 
 
/// enable printing of a leading '+'
///
/// When this manipulator is written to an ostream the a leading
/// '+' will be printed when a positive value is printed.  
constexpr _showpos showpos( true );
  
/// disable printing of a leading '+'
///
/// When this manipulator is written to an ostream the a leading
/// '+' will be printed when a positive value is printed.  
/// This is the initial situation.
constexpr _showpos noshowpos( false );
   
struct _showbase {      
   const bool x;
   constexpr _showbase( bool x ) : x( x ){}
};
           
/// enable printing of a leading radix indication
///
/// When this manipulator is written to an ostream a leading
/// radix indication (0b, 0d, 0o, 0x) will be printed 
/// when a integer value is printed.  
constexpr _showbase showbase( true );

/// enable printing of a leading radix indication
///
/// When this manipulator is written to an ostream no a leading
/// radix indication will be printed when a integer value is printed.   
/// This is the initial situation.
constexpr _showbase noshowbase( false );
   
struct _boolalpha {        
   const bool x;
      constexpr _boolalpha( bool x ): x( x ){}
   };
   
/// print a bool value as '0' or '1'
///
/// When this manipulator is written to an ostream subsequent boolean
/// values will be written as '0' or '1'.  
/// This is the initial situation.           
constexpr _boolalpha boolalpha( true );
   
/// print a bool value as 'false' or 'true'
///
/// When this manipulator is written to an ostream subsequent boolean
/// values will be written as 'false' or 'true'.  
/// This is the initial situation.           
constexpr _boolalpha noboolalpha( false );

/// ostream filler character manipulator
struct setfill {   
   const char x;
      constexpr setfill( char x ) : x( x ){}
};
           
struct _right {
   constexpr _right(){};
}; 
           
/// align an item right in its field
///
/// When this manipulator is written to an ostream subsequent
/// items that are smaller than the field width are aligned
/// right (filled out at the left) in their field width.
/// This is the initial situation.                      
constexpr _right right;
   
struct _left {
   constexpr _left(){};
}; 
                 
/// align an item left in its field
///
/// When this manipulator is written to an ostream subsequent
/// items that are smaller than the field width are aligned
/// left (filled out at the right) in their field width.                     
constexpr _left left;
            
struct _flush {
   constexpr _flush( void ){};
}; 
          
/// flush an stream
///
/// Writing this manipulator to an ostream has the same
/// effect as calling its flush() function.           
constexpr _flush flush;
   


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
class ostream : public noncopyable {
private:
   
   uint_fast16_t field_width;
   uint_fast16_t numerical_radix;
   char fill_char;
   char hex_base;
   bool align_right;
   bool show_pos;
   bool bool_alpha;
   bool show_base;
  
   static size_t strlen( const char *s ){
      size_t n = 0;
      while( *s != '\0' ){
         n++;
         s++;
      }
      return n; 
   }    
   
   // must handle negative numbers!
   void filler( int_fast16_t n ){
      while( n-- > 0 ){
         *this << fill_char;
      }
   }   
       
               
   // =======================================================================
   //
   // helper for printing integer values, which are generated
   // in reverse order
   //
   // =======================================================================
      
            
   // =======================================================================
   //
   // helper for printing integer values, which are generated
   // in reverse order
   //
   // =======================================================================
      
   struct reverse {
      static constexpr uint_fast16_t length = 70;
      char body[ length ];
      char *content;
         
      reverse(){
         body[ length - 1 ] = '\0';
         content = & body[ length - 1 ];
      }
         
      void add_char( char c ){
         content--;
         *content = c;
      }
         
      void add_digit( char c, char hex_base ){
         if( c > 9 ){
            c += ( hex_base - 10 );
         } else {
            c += '0';
         } 
         add_char( c );
      }
         
      void add_prefix( const ostream & s ){
         if( s.show_base ){
            switch( s.numerical_radix ){
               case 2  : add_char( 'b' ); break;
               case 8  : add_char( 'o' ); break;
               case 10 : return;
               case 16 : add_char( 'x' ); break;
               default : add_char( '?' ); break; 
            }
            add_char( '0' );
         }
      }          
   };
  
public:
      
   constexpr ostream();
      
   /// char output function
   ///
   /// This function is called by the other functions to output
   /// each character.
   virtual void putc( char c ) = 0;    
       
   /// char output operator 
   ostream & operator<< ( char c );
      
   /// flush 
   ///
   /// This function waits until all characters are realy written
   /// to whatever is their destination.
   ///
   /// Writing the flush constant has the same effect as calling flush().
   virtual void flush( void ) = 0;
      
   friend ostream & operator<< ( ostream & stream, const _flush x );
    
      
      
   // =======================================================================
   //
   // manipulators
   //
   // =======================================================================
  
   /// return the current field width
   uint_fast16_t width( void ) const;
      
   /// set the field width, return the old field width
   uint_fast16_t width( uint_fast16_t x );
    
   friend ostream & operator<< ( ostream & stream, const setw & x );
      
   /// return the numerical radix       
   uint_fast16_t base( void ) const;
      
   /// set the numerical radix, return the old numerical radix  
   uint_fast16_t base( uint_fast16_t x );
           
   friend ostream & operator<< ( ostream & stream, const _setbase & x );
      
   /// return the current showpos setting        
   bool showpos( void ) const;
      
   /// set the showpos setting, return the old showpos setting 
   bool showpos( bool x );
           
   friend ostream & operator<< ( ostream & stream, const _showpos & x );
      
   /// return the current showbase setting        e
   bool showbase( void ) const;
      
   /// set the showbase setting, return the old showbase setting     
   bool showbase( bool x );
     
   friend ostream & operator<< ( ostream & stream, const _showbase & x );
      
   /// return the current boolalpha setting        
   bool boolalpha( void ) const;
      
   /// set the noboolalpha setting, return the old noboolalpha setting       
   bool boolalpha( bool x );
           
   friend ostream & operator<< ( ostream & stream, const _boolalpha & x );
      
   /// return the current fill char setting             
   char fill( void ) const { return fill_char; }
      
   /// set the fill char setting, return the old fill char setting
   char fill( char x );
           
   friend ostream & operator<< ( ostream & stream, const setfill x );
          
   void right( void ){ align_right = true; }      
   friend ostream & operator<< ( ostream & stream, const _right x );
      
   void left( void ){ align_right = false; }     
   friend ostream & operator<< ( ostream & stream, const _left x );

   // bool must_align_right( void ){ return align_right; }
      
      
   // =======================================================================
   //
   // print bool and string types
   //
   // =======================================================================
      
   /// output operator for bool
   friend ostream & operator<< ( ostream & stream, bool x );
  
   /// output operator for const char pointer (literal string)
   friend ostream & operator<< ( ostream & stream, const char *s );
  
  
   // =======================================================================
   //
   // print integers
   //
   // =======================================================================
      
   /// output operator for integer      
   friend ostream & operator<< ( ostream & stream, int x );
   
   /// output operator for short integer   
   friend ostream & operator<< ( ostream & stream, short int x );
   
   /// output operator for long integer   
   friend ostream & operator<< ( ostream & stream, long int x );
   
   /// output operator for long long integer   
   friend ostream & operator<< ( ostream & stream, long long int x );
   
   /// output operator for short unsigned integer   
   friend ostream & operator<< ( ostream & stream, short unsigned int x );
   
   /// output operator for unsigned integer   
   friend ostream & operator<< ( ostream & stream, unsigned int x );
   
   /// output operator for unsigned long integer   
   friend ostream & operator<< ( ostream & stream, unsigned long int x );
 
   /// output operator for unsigned long long integer    
   friend ostream & operator<< ( ostream & stream, unsigned long long x );
   
   /// output operator for signed char (prints as integer)   
   friend ostream & operator<< ( ostream & stream, signed char c );

   /// output operator for unsigned char (prints as integer)   
   friend ostream & operator<< ( ostream & stream, unsigned char c );
      
}; // class ostream  

}; // namespace hwlib

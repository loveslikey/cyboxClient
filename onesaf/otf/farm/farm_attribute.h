/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
/*
 * Classification:  Unclassified
 *
 * Project Name:  OneSAF
 *
 * DISTRIBUTION STATEMENT C.
 *
 * Distribution is authorized to U.S. Government Agencies and their
 * contractors. Export Controlled: 23 August 2012. Other requests for
 * this document shall be referred to U.S. Army PEO STRI.
 *
 */
#ifndef FARM_ATTRIBUTE_H
#define FARM_ATTRIBUTE_H
#include <iostream>
#include <string>

#include "core/sys_types.h"
#include "core/uuid.h"

namespace FARM
{
    typedef int
        AttributeCategory; // Used to identify the type of an attribute
                           // (length, width, STGJ, etc)..

    typedef CORE::Int32
        AttributeCode;

    typedef std::string
        AttributeLabel;

    // Stores the data type for an attribute.
    //
    enum AttributeDataType
    {
        no_data_type, // Should only be used when the FARM is being read or
                      // written.
        int32,
        float64,
        string,
        enumeration,
        boolean,
        uuid,
        deleted     // Used for edcs conversion only
    };

    // Stores the type of units for an attribute.
    //
    enum AttributeUnits
    {
        unitless,
        meters,
        meters_per_second,
        square_meters,
        degrees,
        kilograms,
        kilograms_per_cubic_meter,
        celsius,
        liters,
        lux,
        pascals,
        enumeration_units,
        milliseconds
    };

    // ------------------------------------------------------------------------
    // Attribute Class contains all data for any attribute in the FARM.
    //
    class Attribute
    {
      public:

        // Default constructor.
        //
        Attribute(void);

        // Constructs a Attribute with the given Attribute Code.
        //
        Attribute( const AttributeCode &new_code);

        // Consturcts a attribute with the given Attribute label.
        //
        Attribute( const AttributeLabel &new_label);

        // Copy Constructor.
        //
        Attribute(const Attribute &rhs);

        // Sets tje attribute code for the attribute,
        // clears the rest of the class members, and
        // deteremines the new attribute label for the new code.
        //
        bool set_code( const AttributeCode &new_code );

        // Sets the attribute label for the attribute,
        // clears the rest of the class members, and
        // determines the new attribute code for the new label.
        //
        bool set_label( const AttributeLabel &new_label );

        // Set the data type of the attribute.
        //
        void set_data_type(const AttributeDataType &new_data_type);

        // Sets the units for this attribute.
        //
        void set_units(const AttributeUnits units);

        // Sets whether of not the attribute is editable at runtime.
        //
        void set_editability( const bool &new_editability );

        // Gets the attribute code.
        //
        AttributeCode get_code(void) const;

        // Gets the Attirbute Category, same as Code.
        //
        AttributeCategory get_category(void) const;

        // Gets the attribute Label.
        //
        const AttributeLabel &get_label(void) const;

        // Get the data type.
        //
        AttributeDataType get_data_type( void ) const;

        // Return:  The units for this attribute.
        //
        AttributeUnits get_units() const;

        // Gets the editability of the attribute.
        //
        bool get_editability( void ) const;

        // Has the attribute been initialized with a valid attribtue?
        //
        bool valid() const;

        // Return:  Are the two attributes equal?
        //
        bool operator==(const Attribute &rhs) const;

        // Return:  Are the two attribute not equal?
        //
        bool operator!=(const Attribute &rhs) const;

        // Return: Is the attribute less than the argument attribute?
        // (Compares the attribute code numerically.)
        //
        bool operator<(const Attribute &rhs) const;

        // Sets the attribute to be the same as the argument attribute.
        //
        Attribute &operator=(const Attribute &rhs);

        // Writes the attribute to the output stream.
        //
        void write(std::ostream &stream) const;

        // Reads the attribute in from the input stream.
        //
        void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary
        // stream in a format that can be read by the terrain compiler.
        //
        void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        void load(std::istream &stream);

      private:

        AttributeLabel
            label;
        AttributeCode
            code;
        AttributeDataType
            data_type;
        AttributeUnits
            units;
        bool
            editability;
    };

    // ------------------------------------------------------------------------
    // DoubleAttribute contains a value for a attribute
    // of data type float or double.
    //
    class DoubleAttribute : public Attribute
    {
      public:

        // Constructs DoubleAttribute with the given Label and Value.
        //
        DoubleAttribute(
            const AttributeLabel &new_label,
            const double new_value
        );

        // Copy Constructor.
        //
        DoubleAttribute( const DoubleAttribute &rhs );

        // Sets the value of the DoubelAttribute.
        //
        void set_value( const double new_value );

        // Gets the value of the DoubleAttribute.
        //
        double get_value( void ) const;

      private:
        double
            value;
    };

    // ------------------------------------------------------------------------
    // IntAttribute contains a value for a attribute
    // of data type integer.
    //
    class IntAttribute : public Attribute
    {
      public:

        // Constructs IntAttribute with the given Label and Value.
        //
        IntAttribute(
            const AttributeLabel &new_label,
            const int new_value
        );

        // Copy Constructor.
        //
        IntAttribute( const IntAttribute &rhs );

        // Sets the value of the IntAttribute.
        //
        void set_value( const int new_value );

        // Gets the value of the IntAttribute.
        //
        int get_value( void ) const;

      private:
        int
            value;
    };

    // ------------------------------------------------------------------------
    // StringAttribute contains a value for a attribute
    // of data type string.
    //
    class StrAttribute : public Attribute
    {
      public:
        // Constructs StringAttribute with the given Label and Value.
        //
        StrAttribute(
            const AttributeLabel &new_label,
            const std::string &new_value );

        // Copy Constructor.
        //
        StrAttribute( const StrAttribute &rhs );

        // Sets the value of the StringAttribute.
        //
        void set_value( const std::string &new_value );

        // Gets the value of the StringAttribute.
        //
        std::string get_value( void ) const;

      private:
        std::string
            value;
    };

    // ------------------------------------------------------------------------
    // UUIDAttribute contains a value for a attribute
    // of data type uuid.
    //
    class UUIDAttribute : public Attribute
    {
      public:
        // Constructs UUIDAttribute with the given Label and Value.
        //
        UUIDAttribute(
            const AttributeLabel &new_label,
            const CORE::UUID &new_value );

        // Copy Constructor.
        //
        UUIDAttribute( const UUIDAttribute &rhs );

        // Sets the value of the UUIDingAttribute.
        //
        void set_value( const CORE::UUID &new_value );

        // Gets the value of the UUIDingAttribute.
        //
        CORE::UUID get_value( void ) const;

      private:
        CORE::UUID
            value;
    };

    // ------------------------------------------------------------------------
    // std::cout << operator to allow an attribute to be displayed easily.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const Attribute &attribute );

    // ------------------------------------------------------------------------
    // std::cout << operator to allow an attribute datra type to be
    // displayed easily.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const AttributeDataType &data_type
    );

    // ------------------------------------------------------------------------
    inline Attribute::Attribute( void )
    {
        label = "";
        code = -999;
        data_type = no_data_type;
        units = unitless;
        editability = false;
    }

    // ------------------------------------------------------------------------
    inline Attribute::Attribute(const Attribute &rhs)
    {
        code = rhs.get_code();
        label = rhs.get_label();
        data_type = rhs.get_data_type();
        units = rhs.get_units();
        editability = rhs.get_editability();
    }

    // ------------------------------------------------------------------------
    inline Attribute::Attribute( const FARM::AttributeCode &new_code )
    {
        set_code( new_code );
    }

    // ------------------------------------------------------------------------
    inline Attribute::Attribute( const FARM::AttributeLabel &new_label )
    {
        set_label(new_label);
    }

    // ------------------------------------------------------------------------
    inline FARM::AttributeCode Attribute::get_code( void ) const
    {
        return code;
    }

    // ------------------------------------------------------------------------
    inline FARM::AttributeCategory Attribute::get_category( void ) const
    {
        return code;
    }

    // ------------------------------------------------------------------------
    inline const FARM::AttributeLabel &Attribute::get_label( void ) const
    {
        return label;
    }

    // ------------------------------------------------------------------------
    inline FARM::AttributeDataType Attribute::get_data_type( void ) const
    {
        return data_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeUnits Attribute::get_units() const
    {
        return units;
    }

    // ------------------------------------------------------------------------
    inline bool Attribute::get_editability( void ) const
    {
        return editability;
    }

    // ------------------------------------------------------------------------
    inline bool Attribute::valid( void ) const
    {
        return
            code != -999;
    }

    // ------------------------------------------------------------------------
    inline bool Attribute::operator==( const Attribute &rhs ) const
    {
        return
            //label == rhs.get_label() and
            code == rhs.get_code() and
            data_type == rhs.get_data_type() and
            units == rhs.get_units() and
            editability == rhs.get_editability();
    }

    // ------------------------------------------------------------------------
    inline bool Attribute::operator!=( const Attribute &rhs ) const
    {
        return
            //label != rhs.get_label() or
            code != rhs.get_code() or
            data_type != rhs.get_data_type() or
            units != rhs.get_units() or
            editability != rhs.get_editability();
    }

    // ------------------------------------------------------------------------
    inline bool Attribute::operator<( const Attribute &rhs ) const
    {
        return code < rhs.get_code();
    }

    // ------------------------------------------------------------------------
    inline Attribute &Attribute::operator=(const Attribute &rhs)
    {
        code = rhs.get_code();
        label = rhs.get_label();
        data_type = rhs.get_data_type();
        units = rhs.get_units();
        editability = rhs.get_editability();

        return *this;
    }

    // ------------------------------------------------------------------------
    inline IntAttribute::IntAttribute(
        const AttributeLabel &new_label,
        const int new_value
    ) : Attribute( new_label )
    {
        set_value( new_value );
    }

    // ------------------------------------------------------------------------
    inline IntAttribute::IntAttribute( const IntAttribute &rhs ) :
        Attribute( rhs )
    {
        value = rhs.get_value();
    }

    // ------------------------------------------------------------------------
    inline DoubleAttribute::DoubleAttribute(
        const AttributeLabel &new_label,
        const double new_value
    ) : Attribute( new_label )
    {
        set_value( new_value );
    }

    // ------------------------------------------------------------------------
    inline DoubleAttribute::DoubleAttribute( const DoubleAttribute &rhs ) :
        Attribute( rhs )
    {
        value = rhs.get_value();
    }

    // ------------------------------------------------------------------------
    inline StrAttribute::StrAttribute(
        const AttributeLabel &new_label,
        const std::string &new_value
    ) : Attribute( new_label )
    {
        set_value( new_value );
    }

    // ------------------------------------------------------------------------
    inline StrAttribute::StrAttribute( const StrAttribute &rhs ) :
        Attribute( rhs )
    {
        value = rhs.get_value();
    }

    // ------------------------------------------------------------------------
    inline UUIDAttribute::UUIDAttribute(
        const AttributeLabel &new_label,
        const CORE::UUID &new_value
    ) : Attribute( new_label ), value(new_value)
    {
    }

    // ------------------------------------------------------------------------
    inline UUIDAttribute::UUIDAttribute( const UUIDAttribute &rhs ) :
        Attribute( rhs ), value(rhs.get_value())
    {
    }
}

#endif


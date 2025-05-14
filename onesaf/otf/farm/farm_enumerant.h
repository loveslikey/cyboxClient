/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
#ifndef FARM_ENUMERANT_H
#define FARM_ENUMERANT_H

/*****************************************************************************
**
** File Name:       farm_enumerant.h
**
** Classification:  Unclassified
**
** Project Name:    OneSAF Software Production
**
** Contract Number: #W900KK-09-D-0323
**
** Task Order:      0007
**
** This work was generated under U.S. Government contract W900KK-09-D-0323,
** and the Government has unlimited data rights therein.
**
** Copyrights:      Copyrights 2011, 2012
**                  Science Applications International Corporation
**                  The Aegis Technologies Group
**                  Cornerstone Software Solutions
**                  Cuspy Engineering Solutions
**                  Dignitas Technologies LLC
**                  GameSim Technologies
**                  Productivity Apex
**                  SoarTech Technology
**                  StackFrame LLC
**                  Lockheed Martin Corporation
**                  Northrop Grumman Corporation
**                  All rights reserved.
**
** DISTRIBUTION STATEMENT C:
**
** Distribution is authorized to the United States Government Agencies and
** USG contractors only (Administrative or Operational Use) (02/02) due to
** critical technology. Other requests shall be referred for consideration
** by  the controlling DoD office.  The controlling DoD office is  the U.S.
** Army PEO STRI Product Manager for One Semi-Automated Forces (PM OneSAF).
**
** Organizations:   Science Applications International Corporation
**                  12901 Science Drive, Orlando, FL  32826-3014
**
** Modifications:   N/A
**
*****************************************************************************/

#include <iostream>
#include <string>

#include "core/byte_array.h"
#include "core/core_string.h"
#include "core/sys_types.h"
#include "farm_attribute.h"

namespace FARM
{
    // Code of the enumerant direct from Sedris.
    //
    typedef CORE::Int32
        EnumerantCode;

    // Label of the enumerant, direct from Sedris.
    typedef std::string
        EnumerantLabel;

    // ------------------------------------------------------------------------
    // Enumerant Class contains both the enumerant and the attribute
    // which the enumerant applies.
    //
    class Enumerant
    {
      public:

        // Default constructor.
        //
        Enumerant(void);

        // Constructs a Enumerant with the given attribute and enumerant codes.
        //
        Enumerant(
            const int new_ea_code,
            const int new_ee_code);

        // Constructs a Enumerant with the given attribute and enumerant labels.
        //
        Enumerant(
            const std::string new_ea_label,
            const std::string new_ee_label);

        // Copy constructor.
        //
        Enumerant(const Enumerant &rhs);

        // Sets the enumerant with the attribute and enumerant code given.
        //
        bool set_codes(
            const int new_ea_code,
            const int new_ee_code);

        // Sets the enumerant with the attribute and enumerant labels given.
        //
        bool set_labels(
            const std::string &new_ea_label,
            const std::string &new_ee_label);

        // Gets the attribute code.
        //
        int get_ea_code(void) const;

        // Gets the enumerant code.
        //
        int get_ee_code(void) const;

        // Gets the attribute label.
        //
        const std::string &get_ea_label(void) const;

        // Gets the enumerant label.
        //
        const std::string &get_ee_label(void) const;

        // Has the enumerant been initialized with valid data?
        //
        bool valid() const;

        void write_ascii(std::ostream &stream) const;

        // Are the two enumerations equal?
        //
        bool operator==(const Enumerant &rhs) const;

        // Are the two enumerations not equal?
        //
        bool operator!=(const Enumerant &rhs) const;

        // Is the enumerant less than the argument enumerant?
        // (compared by the attribute code then Enumerant code numerically.)
        //
        bool operator<(const Enumerant &rhs) const;

        // Sets the enumerant equal the argument enumerant.
        //
        Enumerant &operator=(const Enumerant &rhs);

        // Reads/writes the class to/from the binary byte array as big endian.
        // The byte array pointer is positioned immediately after the last byte
        // read or written.  The byte size of the data fields in the class does
        // not affect the number of bytes read or written.
        //
        void to_byte_array(char *&byte_array_ptr) const;
        void from_byte_array(char *&byte_array_ptr);

        // Return:  The number of bytes read or written for the class to/from
        // the binary byte array.
        //
        static int byte_array_size();

        // Writes the enumerant out to an output stream.
        //
        void write(std::ostream &stream) const;

        // Reads the enumerant in from an input stream.
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

        EnumerantLabel
            ee_label;
        AttributeLabel
            ea_label;
        EnumerantCode
            ee_code;
        AttributeCode
            ea_code;
    };

    // ------------------------------------------------------------------------
    // EnumerantAttribute Class contains a value for a enumerant attribute.
    //
    class EnumerantAttribute : public Attribute
    {
      public:
        // Constructs an EnumerantAttribute with the given attribute label and
        // enumerant code.
        //
        EnumerantAttribute(
            const AttributeLabel &new_label,
            const EnumerantCode new_value );

        // Constructs an EnumerantAttribute with the given attribute label and
        // enumerant label.
        //
        EnumerantAttribute(
            const AttributeLabel &new_label,
            const EnumerantLabel &new_value );

        // Copy Constructor.
        //
        EnumerantAttribute( const EnumerantAttribute &rhs );

        // Sets the enumerant value of the enumerant attribute.
        //
        void set_value( const Enumerant new_value );

        // Gets the enumerant value of the enumerant attribute.
        //
        const Enumerant &get_value( void ) const;

      private:
        Enumerant
            value;
    };

    // ------------------------------------------------------------------------
    // std::cout << operator allows for easy display of an Enumerant.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const Enumerant &enumerant
    );

    // ------------------------------------------------------------------------
    inline Enumerant::Enumerant()
    {
        ea_code = -999;
        ee_code = -999;
        //ea_label = "";
        //ee_label = "";
    }

    // ------------------------------------------------------------------------
    inline Enumerant::Enumerant(
        const int new_ea_code,
        const int new_ee_code)
    {
        set_codes( new_ea_code, new_ee_code );
    }

    // ------------------------------------------------------------------------
    inline Enumerant::Enumerant(
        const std::string new_ea_label,
        const std::string new_ee_label)
    {
        set_labels( new_ea_label, new_ee_label );
    }

    // ------------------------------------------------------------------------
    inline Enumerant::Enumerant(const Enumerant &rhs):
    ea_code(rhs.get_ea_code()),
    ee_code(rhs.get_ee_code()),
    ea_label(rhs.get_ea_label()),
    ee_label(rhs.get_ee_label())
    {
    }

    // ------------------------------------------------------------------------
    inline int Enumerant::get_ea_code(void) const
    {
        return ea_code;
    }

    // ------------------------------------------------------------------------
    inline int Enumerant::get_ee_code(void) const
    {
        return ee_code;
    }

    // ------------------------------------------------------------------------
    inline const std::string &Enumerant::get_ea_label(void) const
    {
        return ea_label;
    }

    // ------------------------------------------------------------------------
    inline const std::string &Enumerant::get_ee_label(void) const
    {
        return ee_label;
    }

    // ------------------------------------------------------------------------
    inline bool Enumerant::valid() const
    {
        return
                ea_code  != -999
            and ee_code  != -999
            and ea_label != ""
            and ee_label != "";
    }

    // ------------------------------------------------------------------------
    inline void Enumerant::write_ascii(std::ostream &stream) const
    {
        stream << ee_code << std::endl;
    }

    // ------------------------------------------------------------------------
    inline bool Enumerant::operator==(const Enumerant &rhs) const
    {
        return
                ea_code  == rhs.get_ea_code()
            and ee_code  == rhs.get_ee_code()
            and ea_label == rhs.get_ea_label()
            and ee_label == rhs.get_ee_label();
    }

    // ------------------------------------------------------------------------
    inline bool Enumerant::operator!=(const Enumerant &rhs) const
    {
        return
               ea_code  != rhs.get_ea_code()
            or ee_code  != rhs.get_ee_code()
            or ea_label != rhs.get_ea_label()
            or ee_label != rhs.get_ee_label();
    }

    // ------------------------------------------------------------------------
    inline Enumerant &Enumerant::operator=(
        const Enumerant &rhs
    )
    {
        ea_code  = rhs.get_ea_code();
        ee_code  = rhs.get_ee_code();
        ea_label = rhs.get_ea_label();
        ee_label = rhs.get_ee_label();

        return *this;
    }

    // ------------------------------------------------------------------------
    inline bool Enumerant::operator<(
        const Enumerant &rhs
    ) const
    {
        return
            ea_code <= rhs.get_ea_code() and
            ee_code < rhs.get_ee_code();
    }

    // ------------------------------------------------------------------------
    inline void Enumerant::to_byte_array(char *&byte_array_ptr) const
    {
        CORE::put_int32(byte_array_ptr, ea_code);
        CORE::put_int32(byte_array_ptr, ee_code);
    }

    // ------------------------------------------------------------------------
    inline void Enumerant::from_byte_array(char *&byte_array_ptr)
    {
        bool
            status = true;
        int
            ea_code_tmp,
            ee_code_tmp;

        ea_code_tmp = CORE::get_int32(byte_array_ptr);
        ee_code_tmp = CORE::get_int32(byte_array_ptr);

        status = set_codes( ea_code_tmp, ee_code_tmp );

        if( not status )
        {
            LOG(
                fatal,
                "Enumeration Codes Invalid From Byte Array:  EA_Code: " +
                    CORE::to_string(ea_code) + "; EE_Code: " +
                    CORE::to_string(ee_code));

            ea_code  = -999;
            ee_code  = -999;
            ea_label = "";
            ee_label = "";
        }
    }

    // ------------------------------------------------------------------------
    inline int Enumerant::byte_array_size(void)
    {
        return sizeof(int) + sizeof(int);
    }

    // ------------------------------------------------------------------------
    inline void Enumerant::write(std::ostream &stream) const
    {
        CORE::Int32
            ea_code_tmp = ea_code,
            ee_code_tmp = ee_code;

        stream.write(
            reinterpret_cast<char *>(&ea_code_tmp),
            sizeof(ea_code_tmp));

        stream.write(
            reinterpret_cast<char *>(&ee_code_tmp),
            sizeof(ee_code_tmp));
    }

    // ------------------------------------------------------------------------
    inline void Enumerant::read(std::istream &stream)
    {
        CORE::Int32
            ea_code_tmp,
            ee_code_tmp;

        stream.read(
            reinterpret_cast<char *>(&ea_code_tmp), sizeof(ea_code_tmp));

        stream.read(
            reinterpret_cast<char *>(&ee_code_tmp), sizeof(ee_code_tmp));

        set_codes(ea_code_tmp, ee_code_tmp);
    }

    // ------------------------------------------------------------------------
    inline EnumerantAttribute::EnumerantAttribute(
        const AttributeLabel &new_label,
        const EnumerantCode new_value
    ) : Attribute(new_label)
    {
        set_value( Enumerant( get_code(), new_value ) );
        set_data_type( enumeration );
    }

    // ------------------------------------------------------------------------
    inline EnumerantAttribute::EnumerantAttribute(
        const AttributeLabel &new_label,
        const EnumerantLabel &new_value
    ) : Attribute(new_label)
    {
        set_value( Enumerant( get_label(), new_value ) );
        set_data_type( enumeration );
    }

    // ------------------------------------------------------------------------
    inline EnumerantAttribute::EnumerantAttribute(
        const EnumerantAttribute &rhs
    ) : Attribute(rhs.get_label())
    {
        set_value( rhs.get_value() );
        set_data_type( enumeration );
    }

    // ------------------------------------------------------------------------
    inline void EnumerantAttribute::set_value( const Enumerant new_value )
    {
        value = new_value;
    }

    // ------------------------------------------------------------------------
    inline const Enumerant &EnumerantAttribute::get_value( void ) const
    {
        return value;
    }

    // ------------------------------------------------------------------------
    inline std::ostream &operator<<(
        std::ostream &stream,
        const Enumerant &enumerant
    )
    {
        stream << "[" << enumerant.get_ea_code() << "] " <<
            enumerant.get_ea_label();
        stream << "; [" << enumerant.get_ee_code() << "] " <<
            enumerant.get_ee_label();

        return stream;
    }
}
#endif


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
#include "core/core_string.h"
#include "core/sys_types.h"
#include "farm.h"
#include "farm_attribute.h"


namespace FARM
{
    // ------------------------------------------------------------------------
    std::ostream &operator<<(
        std::ostream &stream,
        const Attribute &attribute )
    {
        stream << "[" << attribute.get_code() <<
            "] " << attribute.get_label();

        return stream;
    }

    // ------------------------------------------------------------------------
    std::ostream &operator<<(
        std::ostream &stream,
        const AttributeDataType &data_type )
    {
        switch ( data_type )
        {
            case no_data_type:
            {
                stream << "No Data Type";
                break;
            }
            case int32:
            {
                stream << "Integer";
                break;
            }
            case float64:
            {
                stream << "Double";
                break;
            }
            case string:
            {
                stream << "String";
                break;
            }
            case enumeration:
            {
                stream << "Enumeration";
                break;
            }
            case boolean:
            {
                stream << "Boolean";
                break;
            }
            case uuid:
            {
                stream << "UUID";
                break;
            }
            case deleted:
            {
                stream << "Deleted";
                break;
            }
            default:
            {
                break;
            }
        }

        return stream;
    }

    // ------------------------------------------------------------------------
    bool Attribute::set_code( const FARM::AttributeCode &new_code )
    {
        code = new_code;
        data_type = no_data_type;
        units = unitless;
        editability = false;

        bool
            status =
                FeatureAttributeMapping::get_attribute_label_from_code(
                    new_code, label);

        if (not status)
        {
            code = -999;
            label = "";
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool Attribute::set_label(
        const FARM::AttributeLabel &new_label
    )
    {
        label = new_label;
        data_type = no_data_type;
        units = unitless;
        editability = false;

        bool
            status = FeatureAttributeMapping::get_attribute_code(new_label, code);

        if (not status)
        {
            code = -999;
            label = "";
        }

        return status;
    }

    // ------------------------------------------------------------------------
    void Attribute::set_data_type(const FARM::AttributeDataType &new_data_type)
    {
        data_type = new_data_type;
    }

    // ------------------------------------------------------------------------
    void Attribute::set_units(const AttributeUnits units)
    {
        this->units = units;
    }

    // ------------------------------------------------------------------------
    void Attribute::set_editability(const bool &new_editability)
    {
        editability = new_editability;
    }

    // ------------------------------------------------------------------------
    void Attribute::write(std::ostream &stream) const
    {
        CORE::Int32
            tmp_code = code,
            tmp_data_type = static_cast<CORE::Int32>(data_type),
            tmp_units = static_cast<CORE::Int32>(units),
            tmp_editability = editability;

        stream.write(reinterpret_cast<char *>(&tmp_code), sizeof(tmp_code));

        stream.write(
            reinterpret_cast<char *>(&tmp_data_type), sizeof(tmp_data_type));

        stream.write(reinterpret_cast<char *>(&tmp_units), sizeof(tmp_units));

        stream.write(
            reinterpret_cast<char *>(&tmp_editability),
            sizeof(tmp_editability));
    }

    // ------------------------------------------------------------------------
    void Attribute::read(std::istream &stream)
    {
        int
            tmp_code,
            tmp_data_type,
            tmp_units,
            tmp_editability;

        stream.read(reinterpret_cast<char *>(&tmp_code), sizeof(tmp_code));

        stream.read(
            reinterpret_cast<char *>(&tmp_data_type), sizeof(tmp_data_type));

        stream.read(reinterpret_cast<char *>(&tmp_units), sizeof(tmp_units));

        stream.read(
            reinterpret_cast<char *>(&tmp_editability),
            sizeof(tmp_editability));

        set_code(tmp_code);
        data_type = static_cast<AttributeDataType>(tmp_data_type);
        units = static_cast<AttributeUnits>(tmp_units);
        editability = tmp_editability;
    }

    // ------------------------------------------------------------------------
    void Attribute::dump(std::ostream &stream) const
    {
        CORE::dump_string(stream, label);

        CORE::Int32
            int32 = code;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = data_type;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = units;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = editability ? 1 : 0;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // ------------------------------------------------------------------------
    void Attribute::load(std::istream &stream)
    {
        CORE::Int32
            int32;

        CORE::load_string(stream, label);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        code = int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        data_type = static_cast<AttributeDataType>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        units = static_cast<AttributeUnits>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        editability = int32;
    }

    // ------------------------------------------------------------------------
    void DoubleAttribute::set_value( const double new_value )
    {
        value = new_value;
    }

    // ------------------------------------------------------------------------
    double DoubleAttribute::get_value( void ) const
    {
        return value;
    }

    // ------------------------------------------------------------------------
    void IntAttribute::set_value( const int new_value )
    {
        value = new_value;
    }

    // ------------------------------------------------------------------------
    int IntAttribute::get_value( void ) const
    {
        return value;
    }

    // ------------------------------------------------------------------------
    void StrAttribute::set_value( const std::string &new_value )
    {
        value = new_value;
    }

    // ------------------------------------------------------------------------
    std::string StrAttribute::get_value( void ) const
    {
        return value;
    }

    // ------------------------------------------------------------------------
    void UUIDAttribute::set_value( const CORE::UUID &new_value )
    {
        value = new_value;
    }

    // ------------------------------------------------------------------------
    CORE::UUID UUIDAttribute::get_value( void ) const
    {
        return value;
    }
}

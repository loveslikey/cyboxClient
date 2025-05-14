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
#ifndef FARM_DATA_TYPES_H
#define FARM_DATA_TYPES_H
#include <iostream>
#include <list>
#include <set>

#include "core/compare.h"
#include "core/sys_types.h"
#include "farm_enumerant.h"

namespace FARM
{
    typedef CORE::Int32
        AttributeOffset; // Stores the offset for an attribute from the
                         // beginning of the attribute overlay for a feature.

    typedef std::pair<AttributeOffset, AttributeDataType>
        OffsetAndDataType; // Stores the offset and data type for an attribute.

    typedef std::list<OffsetAndDataType>
        OffsetsAndDataTypes; // Stores the offsets and data types for all of
                             // the attributes in a feature.
    typedef std::pair<AttributeCategory, AttributeOffset>
        StringAttribute; // Stores the attribute category and attribute offset
                         // for a string attribute.

    typedef std::set< Enumerant >
        Enumerants; // Stores the valid enumerations for an attribute in a
                    // particular feature type.

    // ------------------------------------------------------------------------
    // Parent class for the various data types stored in the two-dimensional
    // array.  Stores an attributes offset in a feature overlay.
    // ------------------------------------------------------------------------
    class DataType
    {
      public:

        DataType(void);

        virtual ~DataType(void);

        // Return:  The offset of the attribute in a feature overlay.
        //
        AttributeOffset get_offset(void);

        // Sets the offset for the attribute.
        //
        void set_offset(const AttributeOffset new_offset);

        // Writes the data in the class to the stream.
        //
        virtual void write(std::ostream &stream) const;

        // Reads the data in the class from the stream.
        //
        virtual void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary
        // stream in a format that can be read by the terrain compiler.
        //
        virtual void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        virtual void load(std::istream &stream);

      private:

        AttributeOffset
            offset;
    };

    // ------------------------------------------------------------------------
    // Stores the default, minimum, and maximum values and overlay offset for
    // an attribute in a particular feature type whose data type is an Int32 or
    // a Float64.
    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    class InstantiatedDataType : public DataType
    {
      public:

        InstantiatedDataType(void);

        InstantiatedDataType(
            const DatabaseDataType new_def,
            const DatabaseDataType new_min,
            const DatabaseDataType new_max
        );

        // Return:  The minimum value for the attribute.
        //
        DatabaseDataType get_minimum(void) const;

        // Return:  The maximum value for the attribute.
        //
        DatabaseDataType get_maximum(void) const;

        // Return:  The default value for the attribute.
        //
        DatabaseDataType get_default(void) const;

        // Writes the data in the class to the stream.
        //
        virtual void write(std::ostream &stream) const;

        // Reads the data in the class from the stream.
        //
        virtual void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary
        // stream in a format that can be read by the terrain compiler.
        //
        virtual void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        virtual void load(std::istream &stream);

      private:

        // Writes the values in this class instance to the binary stream as
        // Int32's.
        //
        void dump_int32(std::ostream &stream) const;

        // Reads the values in this class instance from the binary stream as
        // Int32's.
        //
        void load_int32(std::istream &stream);

        // Writes the values in this class instance to the binary stream as
        // Float64's.
        //
        void dump_float64(std::ostream &stream) const;

        // Reads the values in this class instance from the binary stream as
        // Float64's.
        //
        void load_float64(std::istream &stream);

        DatabaseDataType
            def,
            min,
            max;
    };

    // ------------------------------------------------------------------------
    // Stores the overlay offset for an attribute in a particular feature type
    // whose data type is a String.
    // ------------------------------------------------------------------------
    class StringDataType : public DataType
    {
      public:

        StringDataType(void);
    };

    // ------------------------------------------------------------------------
    // Stores the overlay offset for an attribute in a particular feature type
    // whose data type is a Boolean.
    // ------------------------------------------------------------------------
    class BooleanDataType : public DataType
    {
      public:
        BooleanDataType(void);
        BooleanDataType(
            const bool value
        );

        bool get_default(void) const;

        virtual void write(std::ostream &stream) const;

        virtual void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary
        // stream in a format that can be read by the terrain compiler.
        //
        virtual void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        virtual void load(std::istream &stream);

      private:

        bool
            def;
    };

    // ------------------------------------------------------------------------
    // Stores the default and valid values and overlay offset for an attribute
    // in a particular feature type whose data type is an Enumerant.
    // ------------------------------------------------------------------------
    class EnumerantDataType : public DataType
    {
      public:

        EnumerantDataType(void);

        EnumerantDataType(
            const Enumerant &default_enum,
            const Enumerants &valid_enumerations
        );

        virtual ~EnumerantDataType(void);

        // Return:  The default value for the attribute.
        //
        EnumerantCode get_default(void) const;

        // Return:  The valid enumerations for the attribute.
        //
        const Enumerants &enumerants(void) const;

        // Writes the data in the class to the stream.
        //
        virtual void write(std::ostream &stream) const;

        // Reads the data in the class from the stream.
        //
        virtual void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary
        // stream in a format that can be read by the terrain compiler.
        //
        virtual void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        virtual void load(std::istream &stream);

      private:

        Enumerant
            def;
        Enumerants
            valid_enums;
    };

    // ------------------------------------------------------------------------
    // Stores the overlay offset for an attribute in a particular feature type
    // whose data type is a UUID.
    // ------------------------------------------------------------------------
    class UUIDDataType : public DataType
    {
      public:

        UUIDDataType(void);
    };

    // -----------------------------------------------------------------------
    inline void FARM::DataType::write(std::ostream &stream) const
    {
        AttributeOffset
            attribute_offset = offset;

        stream.write(
            reinterpret_cast<const char *>(&attribute_offset),
            sizeof(attribute_offset));
    }

    // ------------------------------------------------------------------------
    inline void FARM::DataType::read(std::istream &stream)
    {
        stream.read(reinterpret_cast<char *>(&offset), sizeof(offset));
    }

    // -----------------------------------------------------------------------
    inline void FARM::DataType::dump(std::ostream &stream) const
    {
        CORE::Int32
            int32 = offset;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // -----------------------------------------------------------------------
    inline void FARM::DataType::load(std::istream &stream)
    {
        CORE::Int32
            int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        offset = int32;
    }

    // ------------------------------------------------------------------------
    inline FARM::EnumerantDataType::EnumerantDataType(
        const Enumerant &default_enum,
        const Enumerants &valid_enumerations
    ) : DataType()
    {
        def = default_enum;
        valid_enums = valid_enumerations;

        ASSERT_WITH_STREAM(
            valid_enumerations.size(),
            fatal,
            "The attribute '" << default_enum.get_ea_label() << "' had no "
                "valid enumeration values.");

        ASSERT_WITH_STREAM(
            valid_enumerations.end() != valid_enumerations.find(default_enum),
            fatal,
            "The default enumeration for '" << default_enum.get_ea_label() <<
                "' was not valid.");
    }

    // ------------------------------------------------------------------------
    inline void FARM::EnumerantDataType::write(std::ostream &stream) const
    {
        Enumerants::const_iterator
            iterator = valid_enums.begin();
        CORE::Int32
            num_valid_enums = valid_enums.size();

        DataType::write(stream);

        // Write the default enumeration value.
        //
        def.write(stream);

        // Write the valid enumerations.
        //
        stream.write(
            reinterpret_cast<char *>(&num_valid_enums),
            sizeof(num_valid_enums));

        while (iterator != valid_enums.end())
        {
            iterator->write(stream);

            ++iterator;
        }
    }

    // ------------------------------------------------------------------------
    inline void FARM::EnumerantDataType::read(std::istream &stream)
    {
        Enumerant
            valid_enum;
        CORE::Int32
            num_valid_enums;

        DataType::read(stream);

        // Read the default enumeration value.
        //
        valid_enum.read(stream);
        def = valid_enum;

        // Read the valid enumerations.
        //
        valid_enums.clear();

        stream.read(
            reinterpret_cast<char *>(&num_valid_enums),
            sizeof(num_valid_enums));

        for (int index = 0; index < num_valid_enums; ++index)
        {
            valid_enum = Enumerant();
            valid_enum.read(stream);

            ASSERT(
                valid_enums.insert(valid_enum).second,
                fatal,
                "Could not add an enumeration to a valid enumeration data "
                    "structure!");
        }
    }

    // ------------------------------------------------------------------------
    inline void FARM::EnumerantDataType::dump(std::ostream &stream) const
    {
        Enumerants::const_iterator
            iter = valid_enums.begin();

        DataType::dump(stream);

        // Write the default enumeration value.
        //
        def.dump(stream);

        // Write the valid enumerations.
        //
        CORE::Int32
            int32 = valid_enums.size();

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        while (iter != valid_enums.end())
        {
            iter->dump(stream);

            ++iter;
        }
    }

    // ------------------------------------------------------------------------
    inline void FARM::EnumerantDataType::load(std::istream &stream)
    {
        DataType::load(stream);

        // Read the default enumeration value.
        //
        def.load(stream);

        // Read the valid enumerations.
        //
        CORE::Int32
            num_enums;
        Enumerant
            enum_value;

        valid_enums.clear();

        stream.read(reinterpret_cast<char *>(&num_enums), sizeof(num_enums));

        for (int index = 0; index < num_enums; ++index)
        {
            enum_value.load(stream);

            ASSERT(
                valid_enums.insert(enum_value).second,
                fatal,
                "Could not add an enumeration.");
        }
    }

    // ------------------------------------------------------------------------
    inline DataType::DataType(void)
    {
    }

    // ------------------------------------------------------------------------
    inline DataType::~DataType(void)
    {
    }

    // ------------------------------------------------------------------------
    inline AttributeOffset DataType::get_offset(void)
    {
        return offset;
    }

    // ------------------------------------------------------------------------
    inline void DataType::set_offset(const AttributeOffset new_offset)
    {
        offset = new_offset;
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline InstantiatedDataType<DatabaseDataType>::InstantiatedDataType(void) :
        DataType()
    {
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline InstantiatedDataType<DatabaseDataType>::InstantiatedDataType(
        const DatabaseDataType new_def,
        const DatabaseDataType new_min,
        const DatabaseDataType new_max
    ) : DataType()
    {
        def = new_def;
        min = new_min;
        max = new_max;

        ASSERT(
            min <= max,
            fatal,
            "The valid minimum value for an attribute was greater than its "
                "valid maximum value!");

        ASSERT(
            CORE::ordered(min, def, max),
            fatal,
            "The default value for an attribute was not in the proper range!");
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline DatabaseDataType InstantiatedDataType<DatabaseDataType>::
        get_minimum(void) const
    {
        return min;
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline DatabaseDataType InstantiatedDataType<DatabaseDataType>::
        get_maximum(void) const
    {
        return max;
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline DatabaseDataType InstantiatedDataType<DatabaseDataType>::
        get_default(void) const
    {
        return def;
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::write(
        std::ostream &stream
    ) const
    {
        DatabaseDataType
            def_value = def,
            min_value = min,
            max_value = max;

        DataType::write(stream);

        stream.write(
            reinterpret_cast<const char *>(&def_value), sizeof(def_value));
        stream.write(
            reinterpret_cast<const char *>(&min_value), sizeof(min_value));
        stream.write(
            reinterpret_cast<const char *>(&max_value), sizeof(max_value));
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::read(
        std::istream &stream
    )
    {
        DataType::read(stream);

        stream.read(reinterpret_cast<char *>(&def), sizeof(def));
        stream.read(reinterpret_cast<char *>(&min), sizeof(min));
        stream.read(reinterpret_cast<char *>(&max), sizeof(max));
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::dump(
        std::ostream &stream
    ) const
    {
        DataType::dump(stream);

        if (sizeof(def) == 4)
        {
            dump_int32(stream);
        }
        else if (sizeof(def) == 8)
        {
            dump_float64(stream);
        }
        else
        {
            LOG(fatal, "Found an unsupported data type.");
        }
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::load(
        std::istream &stream
    )
    {
        DataType::load(stream);

        if (sizeof(def) == 4)
        {
            load_int32(stream);
        }
        else if (sizeof(def) == 8)
        {
            load_float64(stream);
        }
        else
        {
            LOG(fatal, "Found an unsupported data type.");
        }
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::dump_int32(
        std::ostream &stream
    ) const
    {
        CORE::Int32
            int32 = static_cast<CORE::Int32>(def);

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = static_cast<CORE::Int32>(min);
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = static_cast<CORE::Int32>(max);
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::load_int32(
        std::istream &stream
    )
    {
        CORE::Int32
            int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        def = static_cast<DatabaseDataType>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        min = static_cast<DatabaseDataType>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        max = static_cast<DatabaseDataType>(int32);
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::dump_float64(
        std::ostream &stream
    ) const
    {
        CORE::Float64
            float64 = static_cast<CORE::Float64>(def);

        stream.write(reinterpret_cast<char *>(&float64), sizeof(float64));

        float64 = static_cast<CORE::Float64>(min);
        stream.write(reinterpret_cast<char *>(&float64), sizeof(float64));

        float64 = static_cast<CORE::Float64>(max);
        stream.write(reinterpret_cast<char *>(&float64), sizeof(float64));
    }

    // ------------------------------------------------------------------------
    template<class DatabaseDataType>
    inline void InstantiatedDataType<DatabaseDataType>::load_float64(
        std::istream &stream
    )
    {
        CORE::Float64
            float64;

        stream.read(reinterpret_cast<char *>(&float64), sizeof(float64));
        def = static_cast<DatabaseDataType>(float64);

        stream.read(reinterpret_cast<char *>(&float64), sizeof(float64));
        min = static_cast<DatabaseDataType>(float64);

        stream.read(reinterpret_cast<char *>(&float64), sizeof(float64));
        max = static_cast<DatabaseDataType>(float64);
    }

    // ------------------------------------------------------------------------
    inline StringDataType::StringDataType(void) : DataType()
    {
    }

    // ------------------------------------------------------------------------
    inline BooleanDataType::BooleanDataType(void) : DataType()
    {
    }

    // ------------------------------------------------------------------------
    inline BooleanDataType::BooleanDataType(const bool value) : DataType()
    {
        def = value;
    }
    // ------------------------------------------------------------------------
    inline void BooleanDataType::write(std::ostream &stream) const
    {
        CORE::Int32
            def_value = def;

        DataType::write(stream);

        stream.write(
            reinterpret_cast<const char *>(&def_value), sizeof(def_value));
    }

    // ------------------------------------------------------------------------
    inline void BooleanDataType::read(std::istream &stream)
    {
        CORE::Int32
            def_value;

        DataType::read(stream);

        stream.read(reinterpret_cast<char *>(&def_value), sizeof(def_value));

        def = def_value;
    }

    // ------------------------------------------------------------------------
    inline void BooleanDataType::dump(std::ostream &stream) const
    {
        DataType::dump(stream);

        CORE::Int32
            int32 = def ? 1 : 0;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // ------------------------------------------------------------------------
    inline void BooleanDataType::load(std::istream &stream)
    {
        CORE::Int32
            int32;

        DataType::load(stream);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        def = int32;
    }

    // ------------------------------------------------------------------------
    inline bool BooleanDataType::get_default(void) const
    {
        return def;
    }

    // ------------------------------------------------------------------------
    inline EnumerantDataType::EnumerantDataType(void) : DataType()
    {
    }

    // ------------------------------------------------------------------------
    inline EnumerantDataType::~EnumerantDataType(void)
    {
        valid_enums.clear();
    }

    // ------------------------------------------------------------------------
    inline EnumerantCode EnumerantDataType::get_default(void) const
    {
        return def.get_ee_code();
    }

    // ------------------------------------------------------------------------
    inline const Enumerants &EnumerantDataType::enumerants(void) const
    {
        return valid_enums;
    }

    // ------------------------------------------------------------------------
    inline UUIDDataType::UUIDDataType(void) : DataType()
    {
    }
}

#endif


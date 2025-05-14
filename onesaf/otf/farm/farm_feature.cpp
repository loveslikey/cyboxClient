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
#include "farm_feature.h"


namespace FARM
{
    // ------------------------------------------------------------------------
    std::ostream &operator<<(
        std::ostream &stream,
        const FeatureGeometry &geometry )
    {
        switch ( geometry )
        {
            case null:
            {
                stream << "Null";
                break;
            }
            case point:
            {
                stream << "Point";
                break;
            }
            case linear:
            {
                stream << "Linear";
                break;
            }
            case areal:
            {
                stream << "Areal";
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
    std::ostream &operator<<(
        std::ostream &stream,
        const UsageBitmask &usages )
    {
        bool
            first = true;

        if ( usages & avenue )
        {
            stream << "avenue";
            first = false;
        }

        if ( usages & aperture )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "aperture";
            first = false;
        }

        if ( usages & building )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "building";
            first = false;
        }

        if ( usages & agriculture_farm )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "agriculture_farm";
            first = false;
        }

        if ( usages & forest )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "forest";
            first = false;
        }

        if ( usages & furniture )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "furniture";
            first = false;
        }

        if ( usages & raisedCombatPos )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "raisedCombatPos";
            first = false;
        }

        if ( usages & dugInCombatPos )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "dugInCombatPos";
            first = false;
        }

        if ( usages & lane )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "lane";
            first = false;
        }

        if ( usages & multiBldg )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "multiBldg";
            first = false;
        }

        if ( usages & LFSmlVehObstacle )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "LFSmlVehObstacle";
            first = false;
        }

        if ( usages & vehObstacle )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "vehObstacle";
            first = false;
        }

        if ( usages & airVehObstacle )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "airVehObstacle";
            first = false;
        }

        if ( usages & urban )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "urban";
            first = false;
        }

        if ( usages & NBC )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "NBC";
            first = false;
        }

        if ( usages & blocksLSmlVehLOS )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "blocksLSmlVehLOS";
            first = false;
        }

        if ( usages & blocksVehLOS )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "blocksVehLOS";
            first = false;
        }

        if ( usages & blocksLOS )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "blocksLOS";
            first = false;
        }

        if ( usages & protectsLSmlVeh )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "protectsLSmlVeh";
            first = false;
        }

        if ( usages & protectsVeh )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "protectsVeh";
            first = false;
        }

        if ( usages & bodyOfWater )
        {
            if ( ! first )
            {
                stream << ", ";
            }

            stream << "bodyOfWater";
            first = false;
        }

        return stream;
    }
}

namespace FARM
{
    bool Feature::set_category( const FARM::FeatureCategory &new_category )
    {
        FARM::Feature
            new_feature;
        bool
            status = true;

        status = FARM::FeatureAttributeMapping::get_feature(
            new_category,
            new_feature );

        if ( status )
        {
            *this = new_feature;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool Feature::set_code_and_geometry(
        const FARM::FeatureCode &new_code,
        const FARM::FeatureGeometry &new_geometry,
        const FARM::FeatureCategory &new_category )
    {
        code = new_code;
        geometry = new_geometry;
        category = new_category;
        usage_bitmask = static_cast<FARM::UsageBitmask>( 0 );
        precedence = 0;
        attributes_overlay_size = 0;

        bool
            status =
                FeatureAttributeMapping::get_feature_label_from_code(
                    new_code, label);

        if (not status)
        {
            category = -999;
            code = -999;
            label = "";
            geometry = FARM::null;
        }

        if (status and category == -999)
        {
            FARM::Feature
                new_feature;

            status = FARM::FeatureAttributeMapping::get_feature(
                label, geometry, new_feature);

            if (status)
            {
                *this = new_feature;
            }
            else
            {
                LOG(
                    medium,
                    "Could not get FARM::Feature from FeatureLabel: " +
                        label + "; and Geometry: " +
                        CORE::to_string( geometry ) );
            }
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool Feature::set_label_and_geometry(
        const FARM::FeatureLabel &new_label,
        const FARM::FeatureGeometry &new_geometry,
        const FARM::FeatureCategory &new_category )
    {
        label = new_label;
        geometry = new_geometry;
        category = new_category;
        usage_bitmask = static_cast<FARM::UsageBitmask>( 0 );
        precedence = 0;
        attributes_overlay_size = 0;

        FARM::FeatureCode
            new_code;
        bool
            status =
                FeatureAttributeMapping::get_feature_code(new_label, new_code);

        if (status)
        {
            status =
                set_code_and_geometry(new_code, new_geometry, new_category);
        }
        else
        {
            LOG(info, "Feature Label Invalid:  EC_Label: " + label);

            category = -999;
            label = "";
            code = -999;
            geometry = FARM::null;
        }

        if (status and category == -999)
        {
            FARM::Feature
                new_feature;

            status = FARM::FeatureAttributeMapping::get_feature(
                label, geometry, new_feature);

            if (status)
            {
                *this = new_feature;
            }
            else
            {
                LOG(
                    medium,
                    "Could not get FARM::Feature from FeatureLabel: " +
                        label + "; and Geometry: " +
                        CORE::to_string( geometry ) );
            }
        }

        return status;
    }

    // ------------------------------------------------------------------------
    void Feature::set_usage_bitmask(
        const FARM::UsageBitmask &new_usage_bitmask )
    {
        usage_bitmask = new_usage_bitmask;
    }

    // ------------------------------------------------------------------------
    void Feature::set_precedence(
        const FARM::FeaturePrecedence &new_precedence )
    {
        precedence = new_precedence;
    }

    // ------------------------------------------------------------------------
    void Feature::set_attributes_overlay_size(
        const int &new_attributes_overlay_size )
    {
        attributes_overlay_size = new_attributes_overlay_size;
    }

    // ------------------------------------------------------------------------
    FARM::FeatureCategory Feature::get_category( void ) const
    {
        return category;
    }

    // ------------------------------------------------------------------------
    FARM::FeatureCode Feature::get_code( void ) const
    {
        return code;
    }

    // ------------------------------------------------------------------------
    FARM::FeatureLabel Feature::get_label( void ) const
    {
        return label;
    }

    // ------------------------------------------------------------------------
    FARM::FeatureGeometry Feature::get_geometry( void ) const
    {
        return geometry;
    }

    // ------------------------------------------------------------------------
    FARM::UsageBitmask Feature::get_usage_bitmask( void ) const
    {
        return usage_bitmask;
    }

    // ------------------------------------------------------------------------
    FARM::FeaturePrecedence Feature::get_precedence( void ) const
    {
        return precedence;
    }

    // ------------------------------------------------------------------------
    int Feature::get_attributes_overlay_size( void ) const
    {
        return attributes_overlay_size;
    }

    // ------------------------------------------------------------------------
    bool Feature::valid( void ) const
    {
        return
    //        label != "" &&
            code != -999 &&
            geometry != FARM::null;
    }

    // ------------------------------------------------------------------------
    bool Feature::operator==( const Feature &rhs ) const
    {
        return
            category == rhs.get_category() &&
            label == rhs.get_label() &&
            code == rhs.get_code() &&
            geometry == rhs.get_geometry() &&
            usage_bitmask == rhs.get_usage_bitmask() &&
            precedence == rhs.get_precedence() &&
            attributes_overlay_size == rhs.get_attributes_overlay_size();
    }

    // ------------------------------------------------------------------------
    bool Feature::operator<( const Feature &rhs ) const
    {
        return category < rhs.get_category();
    }

    // ------------------------------------------------------------------------
    Feature& Feature::operator=(
        const Feature &rhs )
    {
        category = rhs.get_category();
        code = rhs.get_code();
        label = rhs.get_label();
        geometry = rhs.get_geometry();
        usage_bitmask = rhs.get_usage_bitmask();
        precedence = rhs.get_precedence();
        attributes_overlay_size = rhs.get_attributes_overlay_size();

        return *this;
    }

    // ------------------------------------------------------------------------
    void Feature::write(std::ostream &stream) const
    {
        CORE::Int32
            tmp_category = category,
            tmp_code = code,
            tmp_geometry = static_cast<CORE::Int32>(geometry),
            tmp_bitmask = static_cast<CORE::Int32>(usage_bitmask),
            tmp_precedence = precedence,
            tmp_overlay_size = attributes_overlay_size;

        stream.write(
            reinterpret_cast<char *>(&tmp_category), sizeof(tmp_category));

        stream.write(reinterpret_cast<char *>(&tmp_code), sizeof(tmp_code));

        stream.write(
            reinterpret_cast<char *>(&tmp_geometry), sizeof(tmp_geometry));

        stream.write(
            reinterpret_cast<char *>(&tmp_bitmask), sizeof(tmp_bitmask));

        stream.write(
            reinterpret_cast<char *>(&tmp_precedence), sizeof(tmp_precedence));

        stream.write(
            reinterpret_cast<char *>(&tmp_overlay_size),
            sizeof(tmp_overlay_size));
    }

    // ------------------------------------------------------------------------
    void Feature::read(std::istream &stream)
    {
        int
            tmp_category,
            tmp_code,
            tmp_geometry,
            tmp_bitmask,
            tmp_precedence,
            tmp_overlay_size;

        stream.read(
            reinterpret_cast<char *>(&tmp_category), sizeof(tmp_category));

        stream.read(reinterpret_cast<char *>(&tmp_code), sizeof(tmp_code));

        stream.read(
            reinterpret_cast<char *>(&tmp_geometry), sizeof(tmp_geometry));

        stream.read(
            reinterpret_cast<char *>(&tmp_bitmask), sizeof(tmp_bitmask));

        stream.read(
            reinterpret_cast<char *>(&tmp_precedence), sizeof(tmp_precedence));

        stream.read(
            reinterpret_cast<char *>(&tmp_overlay_size),
            sizeof(tmp_overlay_size));

        set_code_and_geometry(
            tmp_code,
            static_cast<FARM::FeatureGeometry>( tmp_geometry ),
            tmp_category );
        usage_bitmask = static_cast<FARM::UsageBitmask>( tmp_bitmask ),
        precedence = tmp_precedence,
        attributes_overlay_size = tmp_overlay_size;
    }

    // ------------------------------------------------------------------------
    void Feature::dump(std::ostream &stream) const
    {
        CORE::Int32
            int32 = category;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        CORE::dump_string(stream, label);

        int32 = code;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = geometry;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = usage_bitmask;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = precedence;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        int32 = attributes_overlay_size;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // ------------------------------------------------------------------------
    void Feature::load(std::istream &stream)
    {
        CORE::Int32
            int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        category = int32;

        CORE::load_string(stream, label);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        code = int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        geometry = static_cast<FeatureGeometry>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        usage_bitmask = static_cast<UsageBitmask>(int32);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        precedence = int32;

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        attributes_overlay_size = int32;
    }

    // ------------------------------------------------------------------------
    std::ostream &operator<<(
        std::ostream &stream,
        const Feature &feature )
    {
        stream << "[" << feature.get_category() <<
            "] " << feature.get_label() << " (" <<
            feature.get_geometry() << ")";

        return stream;
    }
}

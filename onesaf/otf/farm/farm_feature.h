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
#ifndef FARM_FEATURE_H
#define FARM_FEATURE_H
#include <iostream>
#include <list>
#include <string>

#include "farm_attribute.h"

namespace FARM
{
    // Integer feature code direct from Sedris.
    //
    typedef int
        FeatureCode;

    // String feature label direct from Sedris.
    //
    typedef std::string
        FeatureLabel;

    // Used to identify whether a feature is a point, linear, or areal.  The
    // null geometry is used for the feature category that represents all
    // feature categories.
    //
    enum FeatureGeometry
    {
        null,
        point,
        linear,
        areal
    };

    // Used to identify the type of a feature (river, road, airport, etc.).
    // Combination of a feature code or label and a geometry.
    //
    typedef int
        FeatureCategory;

    // Stores the label and geometry for a feature.
    //
    typedef std::pair<FeatureLabel, FeatureGeometry>
        FeatureLabelAndGeometry;

    // Used to identify what algorithms a feature affects.
    // The bitmasks for multiple usages can be ORed together into a single
    // usage bitmask.  This makes the feature checking for a usage just a simple
    // AND operation.
    //
    enum UsageBitmask
    {
        avenue               = 0x0000001,
        aperture             = 0x0000002,
        building             = 0x0000004,
        agriculture_farm     = 0x0000008,
        forest               = 0x0000010,
        furniture            = 0x0000020,
        raisedCombatPos      = 0x0000040,
        dugInCombatPos       = 0x0000080,
        lane                 = 0x0000100,
        multiBldg            = 0x0000200,
        LFSmlVehObstacle     = 0x0000400,
        vehObstacle          = 0x0000800,
        airVehObstacle       = 0x0001000,
        urban                = 0x0002000,
        NBC                  = 0x0004000,
        blocksLSmlVehLOS     = 0x0008000,
        blocksVehLOS         = 0x0010000,
        blocksLOS            = 0x0020000,
        protectsLSmlVeh      = 0x0040000,
        protectsVeh          = 0x0080000,
        bodyOfWater          = 0x0100000,
    };

    // Used to determine which feature is on top when multiple features overlap
    // the same location.  A feature is on top if it has a higher
    // feature precedence than all of the other features at a location.
    // A feature precedence of zero means that a feature feature precedence does
    // not make sense for the feature.  For example, a feature precedence for
    // an administrative area does not make sense, because the
    // administrative area is a mental concept and not a feature made of
    // physical material.  The administrative area can never be on top of
    // another feature.
    //
    typedef int
        FeaturePrecedence;

    // std:;cout << operator allows easy display of a feature geometry.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const FeatureGeometry &geometry );

    // std:;cout << operator allows easy display of a feature UsageBitmask.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const UsageBitmask &usages );

    // Feature class contains all information about a feature in the FARM.
    //
    class Feature
    {
      public:
        // Default constructor.
        //
        Feature( void );

        // Constucts a feature with the feature category given.
        //
        Feature( const FARM::FeatureCategory &new_category );

        // Constructs a feature with the feature code, geometry,
        // and category given.  If the category is not given the
        // categoory is calculated from the FARM, it is usually only
        // given during the construction of the FARM tables.
        //
        Feature(
            const FARM::FeatureCode &new_code,
            const FARM::FeatureGeometry &new_geometry,
            const FARM::FeatureCategory &new_category = -999 );

        // Constructs a feature with the feature label, geometry,
        // and category given.  If the category is not given the
        // categoory is calculated from the FARM, it is usually only
        // given during the construction of the FARM tables.
        //
        Feature(
            const FARM::FeatureLabel &new_label,
            const FARM::FeatureGeometry &new_geometry,
            const FARM::FeatureCategory &new_category = -999 );

        // Copy constructor.
        //
        Feature(const Feature &rhs);

        // Sets the feature category of the feature and re-computes the code
        // and geometry from the FARM with the new category.
        //
        bool set_category( const FARM::FeatureCategory &new_category );

        // Sets the feature code, geometry, and category.
        // If the category is not given the categoory is calculated from
        // the FARM, it is usually only given during the construction of
        // the FARM tables.
        //
        bool set_code_and_geometry(
            const FARM::FeatureCode &new_code,
            const FARM::FeatureGeometry &new_geometry,
            const FARM::FeatureCategory &new_category = -999 );

        // Sets the feature label, geometry, and category.
        // If the category is not given the category is calculated from
        // the FARM, it is usually only given during the construction of
        // the FARM tables.
        //
        bool set_label_and_geometry(
            const FARM::FeatureLabel &new_label,
            const FARM::FeatureGeometry &new_geometry,
            const FARM::FeatureCategory &new_category = -999 );

        // Sets the feature Usage Bitmasks.
        //
        void set_usage_bitmask( const FARM::UsageBitmask &new_usage_bitmask );

        // Sets the feature precedence.
        //
        void set_precedence( const FARM::FeaturePrecedence &new_precedence );

        // Sets the feature Attribute Overlay Size.
        //
        void set_attributes_overlay_size(
            const int &new_attributes_overlay_size );

        // Gets the Feature Category.
        //
        FARM::FeatureCategory get_category( void ) const;

        // Gets the Feature Code.
        //
        FARM::FeatureCode get_code( void ) const;

        // Gets the Feature Label.
        //
        FARM::FeatureLabel get_label( void ) const;

        // Gets the Feature geometry.
        //
        FARM::FeatureGeometry get_geometry( void ) const;

        // Gets the Feature Usage Bitmask.
        //
        FARM::UsageBitmask get_usage_bitmask( void ) const;

        // Gets the Feature Precedence.
        //
        FARM::FeaturePrecedence get_precedence( void ) const;

        // Gets the attribute overlay size.
        //
        int get_attributes_overlay_size( void ) const;

        // Was the feature initialized with valid data?.
        //
        bool valid() const;

        // Are the two enumerations equal?
        //
        bool operator==( const Feature &rhs ) const;

        // Is the feature less than the argument feature?
        // Compares the feature categories numerically.
        //
        bool operator<( const Feature &rhs ) const;

        // Sets the feature equal to the argument feature.
        //
        Feature &operator=( const Feature &rhs );

        // Writes the feature out to an output stream.
        //
        void write(std::ostream &stream) const;

        // Reads the feature in from an input stream.
        //
        void read(std::istream &stream);

        // Writes the contents of this class instance to the given binary stream
        // in a format that can be read by the terrain compiler.
        //
        void dump(std::ostream &stream) const;

        // Reads the contents of this class instance from the given binary
        // stream.
        //
        void load(std::istream &stream);

      private:

        FARM::FeatureCategory
            category;
        FARM::FeatureLabel
            label;
        FARM::FeatureCode
            code;
        FARM::FeatureGeometry
            geometry;
        FARM::UsageBitmask
            usage_bitmask;
        FARM::FeaturePrecedence
            precedence;
        int
            attributes_overlay_size;
    };

    // ------------------------------------------------------------------------
    // std::cout << operator allows easy displya of a feature class.
    //
    std::ostream &operator<<(
        std::ostream &stream,
        const Feature &feature
    );

    // ------------------------------------------------------------------------
    inline Feature::Feature()
    {
        category = -999;
        label = "";
        code = -999;
        geometry = FARM::null;
        usage_bitmask = static_cast<FARM::UsageBitmask>(0);
        precedence = 0;
        attributes_overlay_size = 0;
    }

    // ------------------------------------------------------------------------
    inline Feature::Feature( const FARM::FeatureCategory &new_category )
    {
        set_category( new_category );
    }

    // ------------------------------------------------------------------------
    inline Feature::Feature(
        const FARM::FeatureCode &new_code,
        const FARM::FeatureGeometry &new_geometry,
        const FARM::FeatureCategory &new_category )
    {
        set_code_and_geometry( new_code, new_geometry, new_category );
    }

    // ------------------------------------------------------------------------
    inline Feature::Feature(
        const FARM::FeatureLabel &new_label,
        const FARM::FeatureGeometry &new_geometry,
        const FARM::FeatureCategory &new_category )
    {
        set_label_and_geometry( new_label, new_geometry, new_category );
    }

    // ------------------------------------------------------------------------
    inline Feature::Feature(const Feature &rhs)
    {
        category = rhs.get_category();
        code = rhs.get_code();
        label = rhs.get_label();
        geometry = rhs.get_geometry();
        usage_bitmask = rhs.get_usage_bitmask();
        precedence = rhs.get_precedence();
        attributes_overlay_size = rhs.get_attributes_overlay_size();
    }
}
#endif

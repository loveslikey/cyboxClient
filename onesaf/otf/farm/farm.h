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
#ifndef FARM_H
#define FARM_H
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "farm_data_types.h"
#include "farm_attribute.h"
#include "farm_feature.h"
#include "farm_enumerant.h"

#include "core/angle.h"
#include "core/linear.h"
#include "core/speed.h"
#include "core/sys_types.h"

#include "core/uuid.h"

namespace FARM
{
    // Enumerant values are defined as a pair consisting of an
    // attribute label and an enumerant label.
    //
    typedef std::pair<FARM::AttributeLabel, FARM::EnumerantLabel>
        AttributeEnumPair;

    // ------------------------------------------------------------------------
    // The FARM is either initialized by reading from configuration files or by
    // reading from the terrain database.  The FDF file contains the label,
    // FACC, geometry, description, and prcedence for all of the feature types
    // in the database.  A two-dimensional array is used to store what
    // attributes the various feature types contain, the valid values for the
    // attributes, and the attribute overlay offsets.  An attribute overlay
    // (character array in memory) is used to store the attributes for a
    // feature.  This overlay provides the fundamental data structure that
    // allows a single class to store any type of feature.
    // ------------------------------------------------------------------------
    class FeatureAttributeMapping
    {
      public:

        // Initializes the FARM using the feature configuration files or the
        // FARM.bin file in the database directory if the configuration files
        // do not exist.
        //
        // Return:  Was the FARM initialized successfully?
        //
        static bool initialize(
            const std::string &database_directory,
            const std::string &fdf_file_label,
            const std::string &adf_file_label,
            const std::string &faa_file_label,
            const std::string &edcs_3p1_4p3_feature_mapping,
            const std::string &edcs_3p1_4p3_attribute_mapping,
            const std::string &edcs_3p1_4p3_enum_mapping
        );

        // Releases any memory that was allocated for the FARM.
        //
        static void destroy(void);

        // Return:  Has the FARM been initialized?
        //
        static bool initialized(void);

        static void verify_farm_initialization(void);

        static bool valid_not_all_feature_category(
            const FARM::FeatureCategory &feature_category);

        // Returns all the features in the FARM.
        //
        // Return:  Were the feature labels and geometries returned
        // successfully?
        //
        static bool get_features(
            std::list<FARM::Feature> &features
        );

        // Returns all the feature label and geometry paris in the FARM.
        //
        // Return:  Were the feature label and geometry paris returned
        // successfully?
        static bool get_feature_label_and_geometries(
            std::list<FARM::FeatureLabelAndGeometry>
                &feature_label_and_geometries
        );

        // Returns all the feature categories in the FARM.
        //
        // Return:  Were the feature labels and geometries returned
        // successfully?
        //
        static bool get_feature_categories(
            std::list<FARM::FeatureCategory> &feature_categories
        );

        // Returns the feature associated with the feature label and geometry.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature(
            const FARM::FeatureLabel &feature_label,
            const FARM::FeatureGeometry &feature_geometry,
            FARM::Feature &feature
        );

        // Returns the feature category associated with the
        // feature label and geometry.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature_category(
            const FARM::FeatureLabel &feature_label,
            const FARM::FeatureGeometry &feature_geometry,
            FARM::FeatureCategory &feature_category
        );

        static FARM::FeatureCategory get_feature_category(
            const FARM::FeatureLabel &feature_label,
            const FARM::FeatureGeometry &feature_geometry
        );

        // Returns the feature associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature(
            const FARM::FeatureCategory &feature_category,
            FARM::Feature &feature
        );

        // Returns the feature label associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature_label(
            const FARM::FeatureCategory &feature_category,
            FARM::FeatureLabel &feature_label
        );

        // Returns the feature code associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature_code(
            const FARM::FeatureCategory &feature_category,
            FARM::FeatureCode &feature_code
        );

        // Returns the respective enumerant label for the given feature code.
        //
        static bool get_feature_label_from_code(
            const FeatureCode &code,
            FeatureLabel &label
        );

        // Returns the respective feature code for the given feature label.
        //
        static bool get_feature_code(
            const FeatureLabel &label,
            FeatureCode &code
        );

        // Returns the feature geometry associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature_geometry(
            const FARM::FeatureCategory &feature_category,
            FARM::FeatureGeometry &feature_geometry
        );

        // Returns the usage bitmask associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_usage_bitmask(
            const FARM::FeatureCategory &feature_category,
            FARM::UsageBitmask &usage_bitmask
        );

        // Returns the feature precedence associated with the feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_feature_precedence(
            const FARM::FeatureCategory &feature_category,
            FARM::FeaturePrecedence &feature_precedence
        );

        // Returns the attributes overlay size associated with the
        // feature category.
        //
        // Return:  Was the feature returned successfully?
        //
        static bool get_attributes_overlay_size(
            const FARM::FeatureCategory &feature_category,
            int &attributes_overlay_size
        );

        // Return:  Is the feature category valid for the terrain database?
        //
        static bool valid_feature(
            const FARM::Feature &feature
        );

        // Return:  Is the feature category valid for the terrain database?
        //
        static bool valid_feature_category(
            const FARM::FeatureCategory &feature_category
        );

        // Returns the feature category used to identify all of the feature
        // types in the TCDM.  This feature category can be used to get all of
        // the features in a slice and so on.
        //
        // Return:  Was the feature category used to identify all of the
        // feature types in the TCDM returned successfully?
        //
        static bool get_all_features_category(
            FARM::FeatureCategory &feature_category
        );

        // A certain feature category is used to identify all of the feature
        // types in the TCDM.  This feature category can be used to get all of
        // the features in a slice and so.
        //
        // Return:  Is the feature category used to identify all of the feature
        // types in the TCDM?
        //
        static bool is_all_features(
            const FARM::FeatureCategory &feature_category);

        // Returns all attributes in the FARM.
        //
        // Return:  Were the attributes returned successfully?
        static bool get_all_attributes(
            std::list<Attribute> &attributes
        );

        // Returns all attribute categories in the FARM.
        //
        // Return:  Were the attributes returned successfully?
        static bool get_all_attribute_categories(
            std::list<FARM::AttributeCategory> &attribute_categories
        );

        // Returns the attribute associated with the attribute label.
        //
        // Return:  Was the attribute category returned successfully?
        //
        static bool get_attribute(
            const FARM::AttributeLabel &attribute_label,
            FARM::Attribute &attribute
        );

        // Returns the attribute category associated with the attribute label.
        //
        // Return:  Was the attribute category returned successfully?
        //
        static bool get_attribute_category(
            const FARM::AttributeLabel &attribute_label,
            FARM::AttributeCategory &attribute_category
        );

        // Returns the attribute associated with the attribute category.
        //
        // Return:  Was the attribute returned successfully?
        //
        static bool get_attribute(
            const AttributeCategory &attribute_category,
            Attribute &attribute
        );

        // Returns the attribute label associated with the attribute category.
        //
        // Return:  Was the attribute label returned successfully?
        //
        static bool get_attribute_label(
            const AttributeCategory &attribute_category,
            AttributeLabel &attribute_label
        );

        // Returns the respective attribute label for the given attribute code.
        //
        static bool get_attribute_label_from_code(
            const AttributeCode &code,
            AttributeLabel &label
        );

        // Returns the respective attribute code for the given attribute label.
        //
        static bool get_attribute_code(
            const AttributeLabel &label,
            AttributeCode &code
        );

        // Returns the data type associated with the attribute category.
        //
        // Return:  Was the data type returned successfully?
        //
        static bool get_data_type(
            const AttributeCategory &attribute_category,
            AttributeDataType &data_type
        );

        // Returns the attribute units associated with the attribute category.
        //
        // Return:  Were the attribute units returned successfully?
        //
        static bool get_attribute_units(
            const AttributeCategory &attribute_category,
            AttributeUnits &attribute_units
        );

        // Returns the attribute editability associated with the
        // attribute category.
        //
        // Return:  Is the attribute category editable?
        //
        static bool is_editable(const AttributeCategory &attribute_category);

        // Return:  Does a feature with the feature category contain the given
        // attribute?
        //
        static bool contains_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category
        );

        // Returns all the attributes in a feature with the feature category.
        //
        // Return:  Were the attribute categories returned successfully?
        //
        static bool get_attributes(
            const FeatureCategory &feature_category,
            std::list<Attribute> &attributes
        );

        // Returns all the attributes in a feature with the feature category.
        //
        // Return:  Were the attribute categories returned successfully?
        //
        static bool get_attribute_labels(
            const FeatureCategory &feature_category,
            std::list<AttributeLabel> &attribute_labels
        );

        // Returns all the attribute categories in a feature with the
        // feature category.
        //
        // Return:  Were the attribute categories returned successfully?
        //
        static bool get_attribute_categories(
            const FeatureCategory &feature_category,
            std::list<AttributeCategory> &attribute_categories
        );

        // Returns the attribute categories and offsets for all of the
        // attributes in a feature with the feature category that are strings.
        //
        // Return:  Were the string attributes returned successfully?
        //
        static bool get_string_attributes(
            const FeatureCategory &feature_category,
            std::list<StringAttribute> &string_attributes
        );

        // Return:  Is the attribute category valid for the terrain database?
        //
        static bool valid_attribute_category(
            const AttributeCategory &attribute_category
        );

        // Return:  Is the attribute category valid for the terrain database?
        //
        static inline bool valid_attribute(
            const Attribute &attribute
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const int attribute_value
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const double attribute_value
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const std::string &attribute_value
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const Enumerant &attribute_value
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const bool attribute_value
        );

        // Return:  Is the attribute value valid for the given feature and
        // attribute?
        //
        static bool valid_attribute(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const CORE::UUID &attribute_value
        );

        // Returns the default attribute value for the given feature and
        // attribute.
        //
        // Return:  Was the default attribute value returned successfully?
        //
        static bool get_default(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            int &default_value
        );

        // Returns the default attribute value for the given feature and
        // attribute.
        //
        // Return:  Was the default attribute value returned successfully?
        //
        static bool get_default(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            double &default_value
        );

        // Returns the default attribute value for the given feature and
        // attribute.
        //
        // Return:  Was the default attribute value returned successfully?
        //
        static bool get_default(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            bool &default_value
        );

        // Returns the default attribute value for the given feature and
        // attribute.
        //
        // Return:  Was the default attribute value returned successfully?
        //
        static bool get_default(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            Enumerant &default_value
        );

        // Returns the minimum and maximum attribute values for the given
        // feature and attribute.
        //
        // Return:  Were the minimum and maximum attribute values returned
        // successfully?
        //
        static bool get_min_max(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            int &minimum_value,
            int &maximum_value
        );

        // Returns the minimum and maximum attribute values for the given
        // feature and attribute.
        //
        // Return:  Were the minimum and maximum attribute values returned
        // successfully?
        //
        static bool get_min_max(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            double &minimum_value,
            double &maximum_value
        );

        // Returns the valid enumerations for the given feature and
        // attribute.
        //
        // Return:  Were the valid enumeration strings returned successfully?
        //
        static bool get_valid_enumerants(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            std::list<Enumerant> &enumerations
        );

        // Returns the valid enumerations for the given feature and
        // attribute.
        //
        // Return:  Were the valid enumeration strings returned successfully?
        //
        static bool get_valid_enumerant_strings(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            std::list<EnumerantLabel> &enumeration_strings
        );

        // Returns the enumeration value associated with the enumeration
        // string.
        //
        // Return:  Was the enumeration value returned successfully?
        //
        static bool get_enumeration_value(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const EnumerantLabel &enumeration_string,
            Enumerant &enumeration_value
        );

        // Returns the enumeration value associated with the enumeration
        // string.
        //
        // Return:  Was the enumeration value returned successfully?
        //
        static bool get_enumeration_value(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            const EnumerantCode &enumerant_code,
            Enumerant &enumeration_value
        );

        // Returns the respective enumerant label for the given
        // attribute label and enumerant code combined.
        //
        static bool get_enumerant_label(
            const AttributeLabel &attrib_label,
            const EnumerantCode &enumerant_code,
            EnumerantLabel &enumerant_label
        );

        // Returns the respective enumerant code for the given
        // attribute label and enumerant label combined.
        //
        static bool get_enumerant_code(
            const AttributeLabel &attrib_label,
            const EnumerantLabel &enumerant_label,
            EnumerantCode &enumerant_code
        );

        // Returns the offset of the attribute from the beginning of the
        // attributes overlay for a feature with the feature category.
        //
        // Return:  Was the attribute offset returned successfully?
        //
        static bool get_attribute_offset(
            const FeatureCategory &feature_category,
            const AttributeCategory &attribute_category,
            AttributeOffset &attribute_offset
        );

        // Returns the overlay offsets and data types for all of the attributes
        // in a feature with the feature category.
        //
        // Return:  Were the attribute offsets and data types returned
        // successfully?
        //
        static bool get_offsets_and_data_types(
            const FeatureCategory &feature_category,
            OffsetsAndDataTypes &offsets_and_data_types
        );

        // Return:  Was the data for the FARM read successfully?
        //
        static bool read(
            const std::string &database_directory, // Directory to read the
                                                   // FARM data from.
            const std::string &config_directory,   // Directory to read config
                                                   // data from.
            std::string &failure_reason
        );

        // Return:  Was the data in the FARM written successfully?
        //
        static bool write(
            const std::string &database_directory  // Directory to write the
                                                   // FARM data to.
        );

        /**
         * Writes the contents of the FARM to the binary file "FARM.bin" in the
         * given directory.  The binary file is in a format that can be read by
         * the terrain compiler.
         *
         * @param output_dir Directory to write the FARM binary file to.
         *
         * @return Was the binary file for the FARM created successfully?
         */
        static bool dump(const std::string &output_dir);

        /**
         * Returns whether features in the given feature category are supposed
         * to be integrated in the ITIN.
         *
         * @param feature_category Are features in this feature category
         *                         supposed to be integrated?
         *
         * @return Are features in the given feature category supposed to be
         *         integrated in the ITIN?
         */
        static bool integrated_feature(
            const FeatureCategory &feature_category
        );

        /**
         * Returns whether the given linear feature category is supposed to be
         * integrated in the ITIN.
         *
         * @param feature_category Is this linear feature category supposed to
         *                         be integrated in the ITIN?
         *
         * @return Is the given linear feature category supposed to be
         *         integrated in the ITIN?
         */
        static bool integrated_linear(const FeatureCategory &feature_category);

        /**
         * Returns "Is the input FeatureCategory one of the 'bridge' types?"
         *
         * The 'bridge' types are BRIDGE, OVERPASS, CAUSEWAY, and ENGINEER_BRIDGE,
         * all of which get BridgeModels.
         *
         * @param feature_category FeatureCategory being tested.
         *
         * @return Is feature_category a member of the group?
         */
        static bool is_bridge(const FeatureCategory &feature_category);

        /**
         * Returns "Do features of the input FeatureCategory have
         *         BRIDGE_SPANs and/or BRIDGE_PIERs?"
         *  
         * The feature types which can have related BRIDGE_SPANs and/or
         * BRIDGE_PIERs are BRIDGEs, OVERPASSes, and CAUSEWAYs.
         * This group specifically does NOT include ENGINEER_BRIDGEs.
         *
         * @param feature_category FeatureCategory being tested.
         *
         * @return Is feature_category a member of the group?
         */
        static bool has_spans_or_piers(const FeatureCategory &feature_category);

        /**
         * Returns whether the given feature category is for a tunnel.
         *
         * @param feature_category Is this feature category for a tunnel?
         *
         * @return Was the given feature category for a tunnel?
         */
        static bool is_tunnel(const FeatureCategory &feature_category);

        /**
         * Initializes the EDCS-related maps; labels-to-codes and codes-to-labels
         * for features, attributes, and enums.
         *
         * @param edcs_feature_mapping_filename file containing feature data
         * @param edcs_attribute_mapping_filename file containing attribute data
         * @param edcs_attrib_enum_mapping_filename file containing enum data
         *
         * @return Were the maps created?
         */
        static bool initialize_edcs_maps(
            const std::string &edcs_feature_mapping_filename,
            const std::string &edcs_attribute_mapping_filename,
            const std::string &edcs_attrib_enum_mapping_filename
        );

      private:

        static void read_fdf(const std::string &fdf_file_label);

        static void read_adf(const std::string &adf_file_label);

        static bool read_attribute_ranges(
            std::ifstream &faa_file,
            std::string &attribute_label,
            std::string &new_geometry,
            const FeatureCategory &feature_category
        );

        static void calculate_offsets_overlay_size(
            const FeatureCategory &feature_category
        );

        static void read_faa(const std::string &faa_file_label);

        static void read_farm_table(std::istream &stream);

        static void write_farm_table(std::ostream &stream);

        static void dump_feature_labels_geometries_to_categories(
            std::ostream &stream
        );

        static void load_feature_labels_geometries_to_categories(
            std::istream &stream
        );

        static void dump_feature_categories_to_features(std::ostream &stream);

        static void load_feature_categories_to_features(std::istream &stream);

        static void dump_attribute_codes_to_attributes(std::ostream &stream);

        static void load_attribute_codes_to_attributes(std::istream &stream);

        static void dump_attribute_codes_to_enums(std::ostream &stream);

        static void load_attribute_codes_to_enums(std::istream &stream);

        static void dump_farm_table(std::ostream &stream);

        static void load_farm_table(std::istream &stream);

        static void initialize_farm_from_binary_file(
            const std::string &database_directory
        );

        static const FARM::Feature *get_feature(
            const FeatureLabel &feature_label,
            const FeatureGeometry &feature_geometry
        );

        static const FARM::Feature *get_feature(
            const FeatureCategory &feature_category
        );

        static bool
            farm_initialized; // Has the FARM been initialized?

        typedef std::vector<FARM::DataType *>
            FarmAttributeCodeToDataType;

        // Two-dimensional array that stores the feature to attribute mappings
        // for the FARM, valid attribute values, and attribute offsets.  Access
        // by farm[feature_category][attribute_category].  A null value means
        // that the feature does not contain the given attribute.
        //
        static std::vector<FarmAttributeCodeToDataType>
            farm;

        typedef std::vector<FARM::Attribute>
            AttributeCodesToAttributes;

        static AttributeCodesToAttributes
            attribute_codes_to_attributes; // Maps attribute categories to
                                           // attributes.
    };

    // ------------------------------------------------------------------------
    inline bool FeatureAttributeMapping::initialized( void )
    {
        return farm_initialized;
    }

    // ------------------------------------------------------------------------
    // Verifies that the FARM has been initialized.
    //
    inline void FeatureAttributeMapping::verify_farm_initialization(void)
    {
        ASSERT(
            FARM::FeatureAttributeMapping::initialized(),
            fatal,
            "The FARM was used before it was initialized!");
    }

    // ------------------------------------------------------------------------
    inline bool FeatureAttributeMapping::get_data_type(
        const AttributeCategory &attribute_category,
        AttributeDataType &data_type
    )
    {
        data_type =
            attribute_codes_to_attributes[attribute_category].get_data_type();

        return true;
    }

    // ------------------------------------------------------------------------
    inline bool FeatureAttributeMapping::contains_attribute(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category
    )
    {
        return
            CORE::ordered(
                0, feature_category, static_cast<int>(farm.size() - 1)) and
            CORE::ordered(
                0,
                attribute_category,
                static_cast<int>(farm[feature_category].size() - 1)) and
            farm[feature_category][attribute_category];
    }

    // ------------------------------------------------------------------------
    inline bool FeatureAttributeMapping::get_attribute_offset(
        const FeatureCategory &feature_category,
        const AttributeCategory &attribute_category,
        AttributeOffset &attribute_offset
    )
    {
        bool
            successful =
                contains_attribute(feature_category, attribute_category);

        if (successful)
        {
            attribute_offset =
                farm[feature_category][attribute_category]->get_offset();
        }

        return successful;
    }
}

#endif


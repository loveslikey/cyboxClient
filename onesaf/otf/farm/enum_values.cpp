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
#include "attribute_categories.h"
#include "enum_values.h"
#include "feature_categories.h"

namespace
{
    // ------------------------------------------------------------------------
    // Return:  The enumeration value for the given enumeration label.
    //
    FARM::Enumerant get_enum_value(
        const FARM::FeatureCategory &feature_category,
        const FARM::AttributeCategory &attribute_category,
        const FARM::EnumerantLabel &enum_label
    )
    {
        FARM::Enumerant
            enum_value;

        ASSERT(
            FARM::FeatureAttributeMapping::get_enumeration_value(
                feature_category, attribute_category, enum_label, enum_value),
            fatal,
            "Could not get the enum value for '" + enum_label + "'.");

        return enum_value;
    }
}

namespace FARM
{
    bool
        EnumValues::initialized = false;

    Enumerant
        EnumValues::my_perennial_or_permanent;

    // ------------------------------------------------------------------------
    bool EnumValues::initialize()
    {
        if (FeatureAttributeMapping::initialized())
        {
            initialized = true;

            my_perennial_or_permanent = get_enum_value(
                FeatureCategories::river(),
                AttributeCategories::hydrologic(),
                "PERENNIAL_OR_PERMANENT");
        }

        return FeatureAttributeMapping::initialized();
    }
}


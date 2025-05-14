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
#include "farm_enumerant.h"
#include "farm.h"

namespace FARM
{
    // ------------------------------------------------------------------------
    bool Enumerant::set_codes(const int new_ea_code, const int new_ee_code)
    {
        ea_code = new_ea_code;
        ee_code = new_ee_code;

        // Get the attribute label and enumerant label.
        //
        bool
            status =
                FeatureAttributeMapping::get_attribute_label_from_code(ea_code, ea_label) and
                FeatureAttributeMapping::get_enumerant_label(ea_label, ee_code, ee_label);

        if (not status)
        {
            LOG_WITH_STREAM(
                fatal,
                "Enumeration Codes Invalid:  EA_Label: " << ea_label <<
                    "; EA_Code: " << ea_code << "; EE_Code: " << ee_code);

            ea_code = -999;
            ee_code = -999;
            ea_label = "";
            ee_label = "";
        }

        return status;
    }

    // ------------------------------------------------------------------------
    bool Enumerant::set_labels(
        const std::string &new_ea_label,
        const std::string &new_ee_label
    )
    {
        ea_label = new_ea_label;
        ee_label = new_ee_label;

        // Get the attribute code and enumerant code.
        //
        bool
            status =
                FeatureAttributeMapping::get_attribute_code(ea_label, ea_code) and
                FeatureAttributeMapping::get_enumerant_code(ea_label, ee_label, ee_code);

        if (not status)
        {
            ea_label = "";
            ee_label = "";
            ea_code = -999;
            ee_code = -999;
        }

        return status;
    }

    // ------------------------------------------------------------------------
    void Enumerant::dump(std::ostream &stream) const
    {
        CORE::dump_string(stream, ea_label);

        CORE::Int32
            int32 = ea_code;

        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));

        CORE::dump_string(stream, ee_label);

        int32 = ee_code;
        stream.write(reinterpret_cast<char *>(&int32), sizeof(int32));
    }

    // ------------------------------------------------------------------------
    void Enumerant::load(std::istream &stream)
    {
        CORE::Int32
            int32;

        CORE::load_string(stream, ea_label);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        ea_code = int32;

        CORE::load_string(stream, ee_label);

        stream.read(reinterpret_cast<char *>(&int32), sizeof(int32));
        ee_code = int32;
    }
}

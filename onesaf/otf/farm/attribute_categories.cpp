/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
/*****************************************************************************
**
** File Name:       attribute_categories.cpp
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
** Copyrights:      Copyrights 2006-2016
**                  Leidos
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
** Organizations:   Leidos
**                  12901 Science Drive, Orlando, FL  32826-3014
**
** Modifications:   N/A
**
*****************************************************************************/

#include "attribute_categories.h"

namespace
{
    // ------------------------------------------------------------------------
    // Return:  The attribute category for the attribute label.
    //
    FARM::AttributeCategory get_attribute_category(
        const FARM::AttributeLabel &attribute_label
    )
    {
        FARM::Attribute
            attribute;

        ASSERT(
            FARM::FeatureAttributeMapping::get_attribute(
                attribute_label, attribute),
            fatal,
            "Could not get the attribute for '" + attribute_label + "'.");

        return attribute.get_code();
    }
}

namespace FARM
{
    bool
        AttributeCategories::initialized = false;

    AttributeCategory
        AttributeCategories::my_angle,
        AttributeCategories::my_area,
        AttributeCategories::my_bridge_id,
        AttributeCategories::my_bridge_design,
        AttributeCategories::my_brush_density,
        AttributeCategories::my_terrain_obstacle_type,
        AttributeCategories::my_defensive_pos_count,
        AttributeCategories::my_depth,
        AttributeCategories::my_hull_defilade_depth,
        AttributeCategories::my_turret_defilade_depth,
        AttributeCategories::my_damage,
        AttributeCategories::my_ecosystem_type,
        AttributeCategories::my_height,
        AttributeCategories::my_illuminance,
        AttributeCategories::my_length,
        AttributeCategories::my_usable_length,
        AttributeCategories::my_mass,
        AttributeCategories::my_number_of_spans,
        AttributeCategories::my_numeric_object_id,
        AttributeCategories::my_bridge_height,
        AttributeCategories::my_overhead_clearance,
        AttributeCategories::my_path_count,
        AttributeCategories::my_platoon_cap_avail,
        AttributeCategories::my_railroad_gauge,
        AttributeCategories::my_road_illuminated_width,
        AttributeCategories::my_trafficability_fine,
        AttributeCategories::my_trafficability_medium,
        AttributeCategories::my_underbridge_clearance,
        AttributeCategories::my_road_width,
        AttributeCategories::my_water_depth,
        AttributeCategories::my_standing_water_depth,
        AttributeCategories::my_fordable,
        AttributeCategories::my_hydrologic,
        AttributeCategories::my_width,
        AttributeCategories::my_frozen_water_type,
        AttributeCategories::my_rubble_stability,
        AttributeCategories::my_soil_strength_cone,
        AttributeCategories::my_soil_type,
        AttributeCategories::my_snow_density,
        AttributeCategories::my_snow_depth,
        AttributeCategories::my_snow_only_depth,
        AttributeCategories::my_mean_stem_spacing,
        AttributeCategories::my_terrain_route_type,
        AttributeCategories::my_terrain_roughness,
        AttributeCategories::my_soil_density_dry,
        AttributeCategories::my_soil_water_volume,
        AttributeCategories::my_primary_material_type,
        AttributeCategories::my_surface_material_type,
        AttributeCategories::my_soil_wetness,
        AttributeCategories::my_frozen_soil_layer_bottom_depth,
        AttributeCategories::my_frozen_soil_layer_top_depth,
        AttributeCategories::my_vegetation_type,
        AttributeCategories::my_crown_diameter,
        AttributeCategories::my_inside_diameter,
        AttributeCategories::my_stem_diameter,
        AttributeCategories::my_canopy_bottom,
        AttributeCategories::my_name,
        AttributeCategories::my_object_variant,
        AttributeCategories::my_explosive_mine_density,
        AttributeCategories::my_explosive_mine_type,
        AttributeCategories::my_defensive_position_count,
        AttributeCategories::my_defensive_position_type,
        AttributeCategories::my_completion_fraction,
        AttributeCategories::my_mine_density,
        AttributeCategories::my_season,
        AttributeCategories::my_colouration,
        AttributeCategories::my_classification_name,
        AttributeCategories::my_building_construction_type,
        AttributeCategories::my_religious_designation,
        AttributeCategories::my_building_function,
        AttributeCategories::my_minefield_type,
        AttributeCategories::my_aperture_open,
        AttributeCategories::my_roof_assembly_type,
        AttributeCategories::my_object_base_height,
        AttributeCategories::my_vehicular_speed_limit,
        AttributeCategories::my_vehicle_traffic_flow,
        AttributeCategories::my_terrain_elevation,
        AttributeCategories::my_tunnel_cross_section,
        AttributeCategories::my_associated_text,
        AttributeCategories::my_textual_object_identifier,
        AttributeCategories::my_front_and_axis_reference,
        AttributeCategories::my_point_object_type,
        AttributeCategories::my_surface_temperature,
        AttributeCategories::my_vertical_load_bearing_capacity,
        AttributeCategories::my_snow_depth_category,
        AttributeCategories::my_passage_blocked,
        AttributeCategories::my_ladder_present;


    // ------------------------------------------------------------------------
    bool AttributeCategories::initialize()
    {
        if (FeatureAttributeMapping::initialized())
        {
            initialized = true;

            my_angle                          =
                get_attribute_category("ORIENTATION_ANGLE");
            my_area                           =
                get_attribute_category("AREA");
            my_bridge_id                      =
                get_attribute_category("COMPLEX_COMPONENT_IDENTIFIER");
            my_bridge_design                  =
                get_attribute_category("BRIDGE_DESIGN");
            my_brush_density                  =
                get_attribute_category("BRUSH_DENSITY");
            my_terrain_obstacle_type          =
                get_attribute_category("TERRAIN_OBSTACLE_TYPE");
            my_depth                          =
                get_attribute_category("DEPTH_BELOW_SURFACE_LEVEL");
            my_defensive_pos_count            =
                get_attribute_category("DEFENSIVE_POSITION_COUNT");
            my_hull_defilade_depth            =
                get_attribute_category("HULL_DEFILADE_DEPTH");
            my_turret_defilade_depth          =
                get_attribute_category("TURRET_DEFILADE_DEPTH");
            my_damage                         =
                get_attribute_category("GENERAL_DAMAGE_FRACTION");
            my_ecosystem_type                 =
                get_attribute_category("ECOSYSTEM_TYPE");
            my_height                         =
                get_attribute_category("HEIGHT_ABOVE_SURFACE_LEVEL");
            my_illuminance                    =
                get_attribute_category("ILLUMINANCE");
            my_length                         =
                get_attribute_category("LENGTH");
            my_usable_length                  =
                get_attribute_category("USABLE_LENGTH");
            my_mass                           =
                get_attribute_category("MASS");
            my_number_of_spans                =
                get_attribute_category("BRIDGE_SPAN_COUNT");
            my_numeric_object_id              =
                get_attribute_category("NUMERIC_OBJECT_IDENTIFIER");
            my_bridge_height                  =
                get_attribute_category("OVERALL_BRIDGE_HEIGHT");
            my_overhead_clearance             =
                get_attribute_category("OVERHEAD_CLEARANCE");
            my_underbridge_clearance          =
                get_attribute_category("UNDERBRIDGE_CLEARANCE");
            my_platoon_cap_avail              =
                get_attribute_category(
                    "PLATOON_ACCOMMODATION_AVAILABILITY_COUNT");
            my_trafficability_fine            =
                get_attribute_category("TERRAIN_TRAFFICABILITY_FINE");
            my_path_count                     =
                get_attribute_category("PATH_COUNT");
            my_underbridge_clearance          =
                get_attribute_category("UNDERBRIDGE_CLEARANCE");
            my_railroad_gauge                 =
                get_attribute_category("RAILWAY_GAUGE_CATEGORY");
            my_road_illuminated_width         =
                get_attribute_category("ROAD_ILLUMINATED_WIDTH");
            my_road_width                     =
                get_attribute_category("ROAD_MINIMUM_TRAVELLED_WAY_WIDTH");
            my_water_depth                    =
                get_attribute_category("MEAN_WATER_DEPTH");
            my_fordable                       =
                get_attribute_category("FORDABLE");
            my_standing_water_depth           =
                get_attribute_category("MAXIMUM_STANDING_WATER_DEPTH");
            my_hydrologic                     =
                get_attribute_category("HYDROLOGIC_PERMANENCE");
            my_vegetation_type                =
                get_attribute_category("VEGETATION_TYPE");
            my_crown_diameter                 =
                get_attribute_category("CROWN_DIAMETER");
            my_stem_diameter                  =
                get_attribute_category("STEM_DIAMETER");
            my_inside_diameter                =
                get_attribute_category("INSIDE_DIAMETER");
            my_width                          =
                get_attribute_category("WIDTH");
            my_frozen_water_type              =
                get_attribute_category("FROZEN_SURFACE_COVER_TYPE");
            my_rubble_stability               =
                get_attribute_category("RUBBLE_STABILITY");
            my_crown_diameter                 =
                get_attribute_category("CROWN_DIAMETER");
            my_stem_diameter                  =
                get_attribute_category("STEM_DIAMETER");
            my_canopy_bottom                  =
                get_attribute_category("TREE_CANOPY_BOTTOM_HEIGHT");
            my_name                           =
                get_attribute_category("NAME");
            my_object_variant                 =
                get_attribute_category("OBJECT_VARIANT");
            my_explosive_mine_density         =
                get_attribute_category("EXPLOSIVE_MINE_DENSITY");
            my_explosive_mine_type            =
                get_attribute_category("EXPLOSIVE_MINE_TYPE");
            my_soil_type                      =
                get_attribute_category("SOIL_TYPE");
            my_snow_density                   =
                get_attribute_category("SNOW_DENSITY");
            my_snow_depth                     =
                get_attribute_category("TOTAL_SNOW_ICE_DEPTH");
            my_snow_only_depth                =
                get_attribute_category("SNOW_ONLY_DEPTH");
            my_terrain_route_type             =
                get_attribute_category("LAND_ROUTE_TYPE");
            my_terrain_roughness              =
                get_attribute_category("TERRAIN_ROUGHNESS_ROOT_MEAN_SQUARE");
            my_soil_density_dry               =
                get_attribute_category("SOIL_DENSITY_DRY");
            my_soil_water_volume              =
                get_attribute_category("SOIL_WATER_VOLUME");
            my_primary_material_type          =
                get_attribute_category("PRIMARY_MATERIAL_TYPE");
            my_surface_material_type          =
                get_attribute_category("SURFACE_MATERIAL_TYPE");
            my_soil_wetness                   =
                get_attribute_category("SOIL_WETNESS_CATEGORY");
            my_frozen_soil_layer_bottom_depth =
                get_attribute_category("FROZEN_SOIL_LAYER_BOTTOM_DEPTH");
            my_frozen_soil_layer_top_depth    =
                get_attribute_category("FROZEN_SOIL_LAYER_TOP_DEPTH");
            my_defensive_position_count       =
                get_attribute_category("DEFENSIVE_POSITION_COUNT");
            my_defensive_position_type        =
                get_attribute_category("DEFENSIVE_POSITION_TYPE");
            my_completion_fraction            =
                get_attribute_category("CONSTRUCTION_COMPLETION_FRACTION");
            my_mine_density                   =
                get_attribute_category("EXPLOSIVE_MINE_DENSITY");
            my_minefield_type                 =
                get_attribute_category("MINEFIELD_TYPE");
            my_aperture_open                  =
                get_attribute_category("APERTURE_OPEN");
            my_season                         =
                get_attribute_category("SEASON");
            my_colouration                    =
                get_attribute_category("COLOURATION");
            my_classification_name            =
                get_attribute_category("CLASSIFICATION_NAME");
            my_building_construction_type     =
                get_attribute_category("BUILDING_CONSTRUCTION_TYPE");
            my_religious_designation          =
                get_attribute_category("RELIGIOUS_DESIGNATION");
            my_building_function              =
                get_attribute_category("BUILDING_FUNCTION");
            my_roof_assembly_type             =
                get_attribute_category("ROOF_ASSEMBLY_TYPE");
            my_object_base_height             =
                get_attribute_category("OBJECT_BASE_HEIGHT");
            my_vehicular_speed_limit          =
                get_attribute_category("VEHICULAR_SPEED_LIMIT");
            my_vehicle_traffic_flow           =
                get_attribute_category("VEHICLE_TRAFFIC_FLOW");
            my_terrain_elevation              =
                get_attribute_category("TERRAIN_ELEVATION");
            my_tunnel_cross_section           =
                get_attribute_category("TUNNEL_CROSS_SECTION");
            my_associated_text                =
                get_attribute_category("ASSOCIATED_TEXT");
            my_textual_object_identifier      =
                get_attribute_category("TEXTUAL_OBJECT_IDENTIFIER");
            my_front_and_axis_reference       =
                get_attribute_category("FRONT_AND_AXIS_REFERENCE");
            my_point_object_type              =
                get_attribute_category("POINT_OBJECT_TYPE");
            my_surface_temperature            =
                get_attribute_category("SURFACE_TEMPERATURE");
            my_vertical_load_bearing_capacity =
                get_attribute_category("VERTICAL_LOAD_BEARING_CAPACITY");
            my_snow_depth_category =
                get_attribute_category("SNOW_DEPTH_CATEGORY");
            my_passage_blocked =
                get_attribute_category("PASSAGE_BLOCKED");
            my_ladder_present                 =
                get_attribute_category("LADDER_PRESENT");
        }

        return FeatureAttributeMapping::initialized();
    }
}


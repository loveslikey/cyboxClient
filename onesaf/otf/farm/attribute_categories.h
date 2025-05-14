/*
 * NOTICE: This file is critical to the OneSAF virtual training use case.  
 * Any changes to this file will be subject to an increased review process
 * to ensure that changes do not impact OneSAF virtual training.  Please 
 * coordinate proposed changes with PM OneSAF to avoid delays in integration.
 */
#ifndef ATTRIBUTE_CATEGORIES_H
#define ATTRIBUTE_CATEGORIES_H

/*****************************************************************************
**
** File Name:       attribute_categories.h
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

#include "farm.h"

namespace FARM
{
    class AttributeCategories
    {
      public:

        // Determine and store the attribute categories. The FARM class must be
        // intialized before this is called.
        //
        // Return:  Were the attribute categories initialized successfully?
        //
        static bool initialize();

        // --------------------------------------------------------------------
        // The remaining methods return the attribute category (handle) for
        // the indicated attributes.
        // --------------------------------------------------------------------

        // Return:  category for 'ORIENTATION_ANGLE'.
        //
        static AttributeCategory angle();

        // Return:  category for 'AREA'.
        //
        static AttributeCategory area();

        // Return:  category for 'COMPLEX_COMPONENT_IDENTIFIER'.
        //
        static AttributeCategory bridge_id();

        // Return:  category for 'BRIDGE_DESIGN'.
        //
        static AttributeCategory bridge_design();

        // Return:  category for 'BRUSH_DENSITY'.
        //
        static AttributeCategory brush_density();

        // Return:  category for 'TERRAIN_OBSTACLE_TYPE'.
        //
        static AttributeCategory terrain_obstacle_type();

        // Return:  category for 'DEFENSIVE_POSITION_COUNT'.
        //
        static AttributeCategory defensive_pos_count();

        // Return:  category for 'DEPTH_BELOW_SURFACE_LEVEL'.
        //
        static AttributeCategory depth();

        // Return:  category for 'HULL_DEFILADE_DEPTH'.
        //
        static AttributeCategory hull_defilade_depth();

        // Return:  category for 'TURRET_DEFILADE_DEPTH'.
        //
        static AttributeCategory turret_defilade_depth();

        // Return:  category for 'GENERAL_DAMAGE_FRACTION'.
        //
        static AttributeCategory damage();

        // Return:  category for 'ECOSYSTEM_TYPE'.
        //
        static AttributeCategory ecosystem_type();

        // Return:  category for 'HEIGHT_ABOVE_SURFACE_LEVEL'.
        //
        static AttributeCategory height();

        // Return:  category for 'ILLUMINANCE'.
        //
        static AttributeCategory illuminance();

        // Return:  category for 'LENGTH'.
        //
        static AttributeCategory length();

        // Return:  category for 'USABLE_LENGTH'.
        //
        static AttributeCategory usable_length();

        // Return:  category for 'MASS'.
        //
        static AttributeCategory mass();

        // Return:  category for 'BRIDGE_SPAN_COUNT'.
        //
        static AttributeCategory number_of_spans();

        // Return:  category for 'NUMERIC_OBJECT_IDENTIFIER'.
        //
        static AttributeCategory numeric_object_id();

        // Return:  category for 'OVERALL_BRIDGE_HEIGHT'.
        //
        static AttributeCategory bridge_height();

        // Return:  category for 'OVERHEAD_CLEARANCE'.
        //
        static AttributeCategory overhead_clearance();

        // Return:  category for 'PATH_COUNT'.
        //
        static AttributeCategory path_count();

        // Return:  category for 'PLATOON_ACCOMMODATION_AVAILABLE'.
        //
        static AttributeCategory platoon_cap_avail();

        // Return:  category for 'RAILROAD_GAUGE'.
        //
        static AttributeCategory railroad_gauge();

        // Return:  category for 'ROAD_ILLUMINATED_WIDTH'.
        //
        static AttributeCategory road_illuminated_width();

        // Return:  category for 'TERRAIN_TRAFFICABILITY_FINE'.
        //
        static AttributeCategory trafficability_fine();

        // Return:  category for 'TERRAIN_TRAFFICABILITY_MEDIUM'.
        //
        static AttributeCategory trafficability_medium();

        // Return:  category for 'UNDERBRIDGE_CLEARANCE'.
        //
        static AttributeCategory underbridge_clearance();

        // Return:  category for 'ROAD_MINIMUM_WIDTH'.
        //
        static AttributeCategory road_width();

        // Return:  category for 'MEAN_WATER_DEPTH'.
        //
        static AttributeCategory water_depth();

        // Return:  category for 'MAXIMUM_STANDING_WATER_DEPTH'.
        //
        static AttributeCategory standing_water_depth();
 
        //Return: category for 'FORDABLE'.
        //
        static AttributeCategory fordable();

        // Return:  category for 'HYDROLOGIC_PERMANENCE'.
        //
        static AttributeCategory hydrologic();

        // Return:  category for 'WIDTH'.
        //
        static AttributeCategory width();

        // Return:  category for 'VEGETATION_TYPE'.
        //
        static AttributeCategory vegetation_type();

        // Return:  category for 'CROWN_DIAMETER'.
        //
        static AttributeCategory crown_diameter();

        // Return:  category for 'TREE_CANOPY_BOTTOM_HEIGHT'.
        //
        static AttributeCategory canopy_bottom();

        // Return:  category for 'NAME'.
        //
        static AttributeCategory name();

        // Return:  category for 'OBJECT_VARIANT'.
        //
        static AttributeCategory object_variant();

        // Return:  category for 'EXPLOSIVE_MINE_DENSITY'.
        //
        static AttributeCategory explosive_mine_density();

        // Return:  category for 'EXPLOSIVE_MINE_TYPE'.
        //
        static AttributeCategory explosive_mine_type();

        // Return:  category for 'STEM_DIAMETER'.
        //
        static AttributeCategory stem_diameter();

        // Return:  category for 'INSIDE_DIAMETER'.
        //
        static AttributeCategory inside_diameter();

        // Return:  category for 'FROZEN_WATER_TYPE'.
        //
        static AttributeCategory frozen_water_type();

        // Return:  category for 'RUBBLE_STABILITY'.
        //
        static AttributeCategory rubble_stability();

        // Return:  category for 'SOIL_CONE_INDEX'.
        //
        static AttributeCategory soil_strength_cone();

        // Return:  category for 'SOIL_TYPE'.
        //
        static AttributeCategory soil_type();

        // Return:  category for 'SNOW_DENSITY'.
        //
        static AttributeCategory snow_density();

        // Return:  category for 'SNOW_DEPTH'.
        //
        static AttributeCategory snow_depth();

        // Return:  category for 'SNOW_ONLY_DEPTH'.
        //
        static AttributeCategory snow_only_depth();

        // Return:  category for 'MEAN_STEM_SPACING'.
        //
        static AttributeCategory mean_stem_spacing();

        // Return:  category for 'TERRAIN_ROUTE_TYPE'.
        //
        static AttributeCategory terrain_route_type();

        // Return:  category for 'TERRAIN_ROUGHNESS_ROOT_MEAN_SQUARE'.
        //
        static AttributeCategory terrain_roughness();

        // Return:  category for 'SOIL_DENSITY_DRY'.
        //
        static AttributeCategory soil_density_dry();

        // Return:  category for 'SOIL_WATER_VOLUME'.
        //
        static AttributeCategory soil_water_volume();

        // Return:  category for 'PRIMARY_MATERIAL_TYPE'.
        //
        static AttributeCategory primary_material_type();

        // Return:  category for 'SURFACE_MATERIAL_TYPE'.
        //
        static AttributeCategory surface_material_type();

        // Return:  category for 'SOIL_WETNESS'.
        //
        static AttributeCategory soil_wetness();

        // Return:  category for 'FROZEN_SOIL_LAYER_BOTTOM_DEPTH'.
        //
        static AttributeCategory frozen_soil_layer_bottom_depth();

        // Return:  category for 'FROZEN_SOIL_LAYER_TOP_DEPTH'.
        //
        static AttributeCategory frozen_soil_layer_top_depth();

        // Return:  category for 'DEFENSIVE_POSITION_COUNT'.
        //
        static AttributeCategory defensive_position_count();

        // Return:  category for 'DEFENSIVE_POSITION_TYPE'.
        //
        static AttributeCategory defensive_position_type();

        // Return:  category for 'COMPLETION_FRACTION'.
        //
        static AttributeCategory completion_fraction();

        // Return:  category for 'MINE_DENSITY'.
        //
        static AttributeCategory mine_density();

        // Return:  category for 'MINEFIELD_TYPE'.
        //
        static AttributeCategory minefield_type();

        // Return:  category for 'APERTURE_OPEN'.
        //
        static AttributeCategory aperture_open();

        // Return:  category for 'SEASON'.
        //
        static AttributeCategory season();

        // Return:  category for 'COLOURATION'.
        //
        static AttributeCategory colouration();

        // Return:  category for 'CLASSIFICATION_NAME'.
        //
        static AttributeCategory classification_name();

        // Return:  category for 'BUILDING_CONSTRUCTION_TYPE'.
        //
        static AttributeCategory building_construction_type();

        // Return:  category for 'RELIGIOUS_DESIGNATION'.
        //
        static AttributeCategory religious_designation();

        // Return:  category for 'BUILDING_FUNCTION'.
        //
        static AttributeCategory building_function();

        // Return:  category for 'ROOF_ASSEMBLY_TYPE'.
        //
        static AttributeCategory roof_assembly_type();

        // Return:  category for 'OBJECT_BASE_HEIGHT'.
        //
        static AttributeCategory object_base_height();

        // Return: category for 'VEHICULAR_SPEED_LIMIT'
        //
        static AttributeCategory vehicular_speed_limit();

        // Return: category for 'VEHICLE_TRAFFIC_FLOW'
        //
        static AttributeCategory vehicle_traffic_flow();

        // Return: category for 'TERRAIN_ELEVATION'
        //
        static AttributeCategory terrain_elevation();

        // Return: category for 'TUNNEL_CROSS_SECTION'
        //
        static AttributeCategory tunnel_cross_section();

        // Return: category for 'ASSOCIATED_TEXT'
        //
        static AttributeCategory associated_text();

        // Return: category for 'TEXTUAL_OBJECT_IDENTIFIER'
        //
        static AttributeCategory textual_object_identifier();

        // Return: category for 'FRONT_AND_AXIS_REFERENCE'
        //
        static AttributeCategory front_and_axis_reference();

        // Return: category for 'POINT_OBJECT_TYPE'
        //
        static AttributeCategory point_object_type();

        // Return: category for 'SURFACE_TEMPERATURE'
        //
        static AttributeCategory surface_temperature();

        // Return: category for 'VERTICAL_LOAD_BEARING_CAPACITY'
        //
        static AttributeCategory vertical_load_bearing_capacity();

        // Return: category for 'SNOW_DEPTH_CATEGORY'
        //
        static AttributeCategory snow_depth_category();

        // Return: category for 'PASSAGE_BLOCKED'
        //
        static AttributeCategory passage_blocked();

        // Return: category for 'LADDER_PRESENT'
        //
        static AttributeCategory ladder_present();

      private:

        static bool
            initialized;

        // If the category elements have not been initialized report an error.
        // Used to factor out the test and error report from the various
        // pseudo-attribute methods.
        //
        static void validate();

        static AttributeCategory
            my_angle,
            my_area,
            my_bridge_id,
            my_bridge_design,
            my_brush_density,
            my_terrain_obstacle_type,
            my_defensive_pos_count,
            my_depth,
            my_hull_defilade_depth,
            my_turret_defilade_depth,
            my_damage,
            my_ecosystem_type,
            my_height,
            my_illuminance,
            my_length,
            my_usable_length,
            my_mass,
            my_number_of_spans,
            my_numeric_object_id,
            my_bridge_height,
            my_overhead_clearance,
            my_path_count,
            my_platoon_cap_avail,
            my_railroad_gauge,
            my_road_illuminated_width,
            my_trafficability_fine,
            my_trafficability_medium,
            my_underbridge_clearance,
            my_road_width,
            my_water_depth,
            my_standing_water_depth,
            my_fordable,
            my_hydrologic,
            my_width,
            my_frozen_water_type,
            my_rubble_stability,
            my_soil_strength_cone,
            my_soil_type,
            my_snow_density,
            my_snow_depth,
            my_snow_only_depth,
            my_mean_stem_spacing,
            my_terrain_route_type,
            my_terrain_roughness,
            my_soil_density_dry,
            my_soil_water_volume,
            my_primary_material_type,
            my_surface_material_type,
            my_soil_wetness,
            my_frozen_soil_layer_bottom_depth,
            my_frozen_soil_layer_top_depth,
            my_vegetation_type,
            my_crown_diameter,
            my_inside_diameter,
            my_stem_diameter,
            my_canopy_bottom,
            my_name,
            my_object_variant,
            my_explosive_mine_density,
            my_explosive_mine_type,
            my_defensive_position_count,
            my_defensive_position_type,
            my_completion_fraction,
            my_mine_density,
            my_season,
            my_colouration,
            my_classification_name,
            my_religious_designation,
            my_building_construction_type,
            my_building_function,
            my_minefield_type,
            my_aperture_open,
            my_roof_assembly_type,
            my_object_base_height,
            my_vehicular_speed_limit,
            my_vehicle_traffic_flow,
            my_terrain_elevation,
            my_tunnel_cross_section,
            my_associated_text,
            my_textual_object_identifier,
            my_front_and_axis_reference,
            my_point_object_type,
            my_surface_temperature,
            my_vertical_load_bearing_capacity,
            my_snow_depth_category,
            my_passage_blocked,
            my_ladder_present;
    };

    // ------------------------------------------------------------------------
    inline void AttributeCategories::validate()
    {
        if (not initialized)
        {
            LOG(fatal, "AttributeCategories used before initialization.");
        }
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::angle()
    {
        validate();

        return my_angle;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::area()
    {
        validate();

        return my_area;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::bridge_id()
    {
        validate();

        return my_bridge_id;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::bridge_design()
    {
        validate();

        return my_bridge_design;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::brush_density()
    {
        validate();

        return my_brush_density;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::terrain_obstacle_type()
    {
        validate();

        return my_terrain_obstacle_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::depth()
    {
        validate();

        return my_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::hull_defilade_depth()
    {
        validate();

        return my_hull_defilade_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::turret_defilade_depth()
    {
        validate();

        return my_turret_defilade_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::damage()
    {
        validate();

        return my_damage;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::ecosystem_type()
    {
        validate();

        return my_ecosystem_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::height()
    {
        validate();

        return my_height;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::illuminance()
    {
        validate();

        return my_illuminance;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::length()
    {
        validate();

        return my_length;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::usable_length()
    {
        validate();

        return my_usable_length;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::mass()
    {
        validate();

        return my_mass;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::number_of_spans()
    {
        validate();

        return my_number_of_spans;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::numeric_object_id()
    {
        validate();

        return my_numeric_object_id;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::bridge_height()
    {
        validate();

        return my_bridge_height;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::overhead_clearance()
    {
        validate();

        return my_overhead_clearance;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::path_count()
    {
        validate();

        return my_path_count;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::platoon_cap_avail()
    {
        validate();

        return my_platoon_cap_avail;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::railroad_gauge()
    {
        validate();

        return my_railroad_gauge;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::road_illuminated_width()
    {
        validate();

        return my_road_illuminated_width;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::trafficability_fine()
    {
        validate();

        return my_trafficability_fine;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::trafficability_medium()
    {
        validate();

        return my_trafficability_medium;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::underbridge_clearance()
    {
        validate();

        return my_underbridge_clearance;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::road_width()
    {
        validate();

        return my_road_width;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::water_depth()
    {
        validate();

        return my_water_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::standing_water_depth()
    {
        validate();

        return my_standing_water_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::fordable()
    {
        validate();

        return my_fordable;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::hydrologic()
    {
        validate();

        return my_hydrologic;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::inside_diameter()
    {

        validate();
        return my_inside_diameter;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::vegetation_type()
    {
        validate();

        return my_vegetation_type;
    }
    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::crown_diameter()
    {
        validate();

        return my_crown_diameter;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::stem_diameter()
    {
        validate();

        return my_stem_diameter;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::width()
    {
        validate();

        return my_width;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::frozen_water_type()
    {
        validate();

        return my_frozen_water_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::rubble_stability()
    {
        validate();

        return my_rubble_stability;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::soil_strength_cone()
    {
        validate();

        return my_soil_strength_cone;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::soil_type()
    {
        validate();

        return my_soil_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::snow_density()
    {
        validate();

        return my_snow_density;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::snow_depth()
    {
        validate();

        return my_snow_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::snow_only_depth()
    {
        validate();

        return my_snow_only_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::mean_stem_spacing()
    {
        validate();

        return my_mean_stem_spacing;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::terrain_route_type()
    {
        validate();

        return my_terrain_route_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::terrain_roughness()
    {
        validate();

        return my_terrain_roughness;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::soil_density_dry()
    {
        validate();

        return my_soil_density_dry;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::soil_water_volume()
    {
        validate();

        return my_soil_water_volume;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::primary_material_type()
    {
        validate();

        return my_primary_material_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::surface_material_type()
    {
        validate();

        return my_surface_material_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::soil_wetness()
    {
        validate();

        return my_soil_wetness;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::canopy_bottom()
    {
        validate();

        return my_canopy_bottom;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::name()
    {
        validate();

        return my_name;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::object_variant()
    {
        validate();

        return my_object_variant;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::explosive_mine_density()
    {
        validate();

        return my_explosive_mine_density;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::explosive_mine_type()
    {
        validate();

        return my_explosive_mine_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory
        AttributeCategories::frozen_soil_layer_bottom_depth()
    {
        validate();

        return my_frozen_soil_layer_bottom_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::frozen_soil_layer_top_depth()
    {
        validate();

        return my_frozen_soil_layer_top_depth;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::defensive_position_count()
    {
        validate();

        return my_defensive_position_count;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::defensive_position_type()
    {
        validate();

        return my_defensive_position_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::completion_fraction()
    {
        validate();

        return my_completion_fraction;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::mine_density()
    {
        validate();

        return my_mine_density;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::defensive_pos_count()
    {
        validate();

        return my_defensive_pos_count;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::minefield_type()
    {
        validate();

        return my_minefield_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::aperture_open()
    {
        validate();

        return my_aperture_open;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::season()
    {
        validate();

        return my_season;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::colouration()
    {
        validate();

        return my_colouration;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::classification_name()
    {
        validate();

        return my_classification_name;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::building_construction_type()
    {
        validate();

        return my_building_construction_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::religious_designation()
    {
        validate();

        return my_religious_designation;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::building_function()
    {
        validate();

        return my_building_function;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::roof_assembly_type()
    {
        validate();

        return my_roof_assembly_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::object_base_height()
    {
        validate();

        return my_object_base_height;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::vehicular_speed_limit()
    {
        validate();

        return my_vehicular_speed_limit;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::vehicle_traffic_flow()
    {
        validate();

        return my_vehicle_traffic_flow;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::terrain_elevation()
    {
        validate();

        return my_terrain_elevation;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::tunnel_cross_section()
    {
        validate();

        return my_tunnel_cross_section;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::associated_text()
    {
        validate();

        return my_associated_text;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::textual_object_identifier()
    {
        validate();

        return my_textual_object_identifier;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::front_and_axis_reference()
    {
        validate();

        return my_front_and_axis_reference;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::point_object_type()
    {
        validate();

        return my_point_object_type;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::surface_temperature()
    {
        validate();

        return my_surface_temperature;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::vertical_load_bearing_capacity()
    {
        validate();

        return my_vertical_load_bearing_capacity;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::snow_depth_category()
    {
        validate();

        return my_snow_depth_category;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::passage_blocked()
    {
        validate();

        return my_passage_blocked;
    }

    // ------------------------------------------------------------------------
    inline AttributeCategory AttributeCategories::ladder_present()
    {
        validate();

        return my_ladder_present;
    }
}

#endif


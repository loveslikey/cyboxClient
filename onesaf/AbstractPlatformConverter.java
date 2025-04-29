/*
 * Classification:  Unclassified
 *
 * Project Name:  OneSAF
 *
 * DISTRIBUTION STATEMENT C.
 *
 * Distribution is authorized to U.S. Government Agencies and their
 * contractors. Export Controlled: 23 August, 2012. Other requests for
 * this document shall be referred to U.S. Army PEO STRI.
 *
 */
/*
 * Copyrights 2006-2018
 *               Leidos
 *               Cole Engineering Services
 *               Cornerstone Software Solutions
 *               Dignitas Technologies LLC
 *               Productivity Apex
 *               SimIS
 *               All rights reserved.
 */
package net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.converter.common;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import net.onesaf.core.services.data.dm.rdm.coordination.css.BasicLoadMagicStruct;
import net.onesaf.core.services.data.dm.rdm.coordination.css.CargoMagicStruct;
import net.onesaf.core.services.data.dm.rdm.coordination.css.SupplyThreshold;
import net.onesaf.core.services.data.dm.rdm.coordination.css.SupplyThresholdEnum;
import net.onesaf.core.services.data.dm.rdm.coordination.css.SupplyThresholdFactory;
import net.onesaf.core.services.data.dm.rdm.phys.BasicLoadSupply;
import net.onesaf.core.services.data.dm.rdm.phys.CargoSupply;
import net.onesaf.core.services.odm.entity.Entity;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.AttributeRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.AbstractObjectHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.stream.xdr.XDRInputStream;
import net.onesaf.core.services.sim.interopmgt.hla.common.stream.xdr.XDROutputStream;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.fom.matrex.util.EnumUtil;
import net.onesaf.core.services.sys.bso.BulkPOLSupplyTypeEnum;
import net.onesaf.core.services.sys.bso.MunitionTypeEnum;
import net.onesaf.core.services.sys.bso.SupplyType;
import net.onesaf.core.services.sys.enums.AbstractEnum;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;

import org.apache.log4j.Level;

/**
 * Base for all Platform Converters. This class extends CommonConverter to give
 * access to the often use primitive converters. This also gives access to the
 * reference objects stored in AbstractConverter. All MATREX objects to OneSAF
 * objects converters should extend from this converter.
 *
 * @author $Author$
 * @version $Revision$
 */
public class AbstractPlatformConverter extends AbstractObjectConverter {

    /** Reference to the OneSAFLogger object. */
    private static OneSAFLogger logger =
        (OneSAFLogger) OneSAFLogger.getLogger(AbstractPlatformConverter.class);
    private static boolean isInfoEnabled = logger.isEnabledFor(Level.INFO);
    private static boolean isDebugEnabled = logger.isEnabledFor(Level.DEBUG);

    /** The 'basicLoad' OOS attribute. */
    protected static final String ODM_BASIC_LOAD = "basicLoad";

    /** The 'cargo' OOS attribute. */
    protected static final String ODM_CARGO = "cargo";

    /** The "AmmoState" FOM attribute. */
    protected static final String FOM_AMMO_STATE = "AmmoState";

    /** The "AvailableSupply" FOM attribute. */
    protected static final String FOM_AVAILABLE_SUPPLY = "AvailableSupply";

    /** The "FuelStatus" FOM attribute. */
    protected static final String FOM_FUEL_STATUS = "FuelStatus";

    private EnumUtil enumUtil = null;

    /**
     * Initialize the converter. Any internal initialization should be
     * implemented here. Any subclasses of Platform should add/or delete
     * attributes from the pub sub lists.<br>
     * 
     * @param objHandler AbstractObjectHandler reference to the ObjectHandler.
     */
    @Override
    public void initialize(AbstractObjectHandler objHandler) {

        super.initialize(objHandler);

        enumUtil = EnumUtil.getInstance();
    }

    /**
     * Process the BasicLoad on <code>odmEntity</code> for supplies that may
     * need to be published.
     * 
     * @param odmEntity Entity entity whose supplies may need publishing.
     * @param classHandle RTIHandle handle to the FOM class representative of
     *        odmEntity.
     * @param outputData Map store the XDR encoded data.
     */
    protected void handleODMBasicLoad(Entity odmEntity,
            ObjectClassRTIHandle classHandle,
            Map<AttributeRTIHandle, byte[]> outputData) {

        BasicLoadSupply[] currentSupplies = null;
        List<BasicLoadSupply> fuelList = new ArrayList<BasicLoadSupply>(10);
        List<BasicLoadSupply> ammoList = new ArrayList<BasicLoadSupply>(10);

        if (odmEntity != null
                && odmEntity.getBasicLoad() != null
                && odmEntity.getBasicLoad().getSupplies() != null
                && (pubFOMAttrs.contains(FOM_FUEL_STATUS) || pubFOMAttrs
                        .contains(FOM_AMMO_STATE))) {

            currentSupplies =
                    odmEntity.getBasicLoad().getSupplies()
                    .toArray(new BasicLoadSupply[0]);

            if (currentSupplies != null) {

                for (BasicLoadSupply currentSupply : currentSupplies) {

                    if (currentSupply != null) {

                        String supplyClass =
                                currentSupply.getSupplyType().getSupplyClass()
                                .getName();

                        // if the supply is of type fuel add it to fuel list
                        if (supplyClass.equals("class3")) {

                            fuelList.add(currentSupply);
                        } else if (supplyClass.equals("class5")) {

                            ammoList.add(currentSupply);
                        }
                    }
                }

                if (pubFOMAttrs.contains(FOM_FUEL_STATUS)
                        && fuelList.size() > 0) {

                    handleODMFuel(odmEntity, classHandle, fuelList, outputData);
                }

                if (pubFOMAttrs.contains(FOM_AMMO_STATE) && ammoList.size() > 0) {
 
                    handleODMAmmo(odmEntity, classHandle, ammoList, outputData);
                }
            } else {

                logger.error("AbstractPlatformConverter Missing Entity missing Basic Load supplies"
                        + " in handleODMBasicLoad");
            }
        }
    }

    /**
     * XDR encode the data in <code>supplyMunitionList</code> to a FOM
     * AmmoStateCDT attribute used by Platform objects.
     * 
     * @param odmEntity Entity entity whose ammo is to be published.
     * @param classHandle RTIHandle handle to the FOM class representative of
     *        odmEntity.
     * @param supplyMunitionList the list of fuel to publish.
     * @param outputData Map store the XDR encoded data.
     */
    protected void handleODMAmmo(Entity odmEntity,
            ObjectClassRTIHandle classHandle,
            List<BasicLoadSupply> supplyMunitionList,
            Map<AttributeRTIHandle, byte[]> outputData) {

        MunitionTypeEnum munitionType;
        double quantity;
        double maxValue;
        double percentUnitBasicLoad;

        if (isDebugEnabled) {
            logger.debug("AbstractPlatformConverter handling ODM " + ODM_BASIC_LOAD + " change");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        try {
            outXDR.writeInt(supplyMunitionList.size());

            for (BasicLoadSupply currentSupply : supplyMunitionList) {

                // Write MunitionTypeEDT--Not used in BLCSE case where 
            	// primary identification of entities is via DIS septuplet
                if(hlaManager.getConfig().primaryIdentificationViaDIS){
                	outXDR.writeInt(0);
                }else{
                	munitionType = (MunitionTypeEnum) MunitionTypeEnum
    						.valueOf((currentSupply.getSupplyType()).getName());

    				if (munitionType == null) {
    					logger.errorOnce("got null munitionType from supply: "
    							+ currentSupply.getSupplyType());
    					munitionType = AbstractEnum
    							.getDefaultValue(MunitionTypeEnum.class);
    				}

    				outXDR.writeInt(enumTranslator.getMunitionType(munitionType));
                }
                munitionType = (MunitionTypeEnum) MunitionTypeEnum.valueOf((currentSupply
                        .getSupplyType()).getName());
                if (munitionType==null) {
                    logger.errorOnce("AbstractPlatformConverter got null munitionType from supply: "+currentSupply
                        .getSupplyType());
                    munitionType = AbstractEnum.getDefaultValue(MunitionTypeEnum.class);
                }
                String externalIdentifier = (enumTranslator.getDISMunitionType(munitionType));

                // Write DISMunitionType -
                outXDR.writeInt(1);
                enumUtil.writeDISEnum(outXDR, externalIdentifier);

                // Write Quantity
                quantity = currentSupply.getUsableQuantity();
                outXDR.writeInt((int) quantity);

                // Write PercentUnitBasicLoad
                maxValue = currentSupply.getCapacity();

                // calculate % basic load
                percentUnitBasicLoad = (quantity / maxValue) * 100;
                outXDR.writeDouble(percentUnitBasicLoad);

                if (isDebugEnabled) {
                    logger.debug("AbstractPlatformConverter.handleAmmo " +
                                 munitionType + " " + 
                                 externalIdentifier + " " + 
                                 quantity + " " + 
                                 percentUnitBasicLoad);
                }
            }

            setAttributeInClassHandle(classHandle, FOM_AMMO_STATE,
                outByte.toByteArray(), outputData);
        } catch (IOException ex) {

            logger.error("IOException encountered when handling ODM "
                    + "basicLoad - ammo", ex);
        }
    }

    /**
     * Process the Cargo on <code>odmEntity</code> for supplies that may
     * need to be published.
     * 
     * @param odmEntity Entity entity whose supplies may need publishing.
     * @param classHandle RTIHandle handle to the FOM class representative of
     *        odmEntity.
     * @param outputData Map store the XDR encoded data.
     */
    protected void handleODMCargo(Entity odmEntity,
            ObjectClassRTIHandle classHandle,
            Map<AttributeRTIHandle, byte[]> outputData) {

        if (isDebugEnabled) {
            logger.debug("AbstractPlatformConverter handling ODM " + ODM_CARGO + " change");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        try {
            List cargoSupplies = null;
            if ((odmEntity != null) &&
                (odmEntity.getCargo() != null)) {
                cargoSupplies = odmEntity.getCargo().getSupplies();
            }

            if (cargoSupplies != null) {

                // num of items in the cargo area
                outXDR.writeInt(cargoSupplies.size());
                if (isDebugEnabled) {
                    logger.debug("PlatformConverter.handleODMCargo " + cargoSupplies.size());
                }

                Iterator suppliesIter = cargoSupplies.iterator();

                while (suppliesIter.hasNext()) {
                    CargoSupply item = (CargoSupply)suppliesIter.next();
                    SupplyType supply = item.getSupplyType();
                    double quantity = item.getUsableQuantity();
                    String externalIdentifier = "0.0.0.0.0.0.0";

                    if (supply instanceof MunitionTypeEnum) {
                        MunitionTypeEnum munitionType = (MunitionTypeEnum)supply;
                        externalIdentifier = (enumTranslator.getDISMunitionType(munitionType));
                    } else if (supply instanceof BulkPOLSupplyTypeEnum) {
                        externalIdentifier = enumUtil.pol2String(supply);
                        quantity *= 1000;
                    }

                    enumUtil.writeDISEnum(outXDR, externalIdentifier);

                    // Write UnitofIssue
                    outXDR.writeInt(0); // unknown

                    // Write NumUnits
                    outXDR.writeDouble(quantity);

                    if (isDebugEnabled) {
                        logger.debug("PlatformConverter.handleODMCargo " + 
                                     supply + ":" + quantity + ":" + 
                                     externalIdentifier);
                    }
                }
            }
            setAttributeInClassHandle(classHandle, FOM_AVAILABLE_SUPPLY,
                outByte.toByteArray(), outputData);
        } catch (IOException ex) {

            logger.error("IOException encountered when handling ODM "
                    + "cargo - ammo", ex);
        }
    }

    /**
     * XDR decode a the FOM Ammo_State attribute. 
     * 
     * @param odmEntity that may be dismounted.
     * @param encodedData byte[] XDR encoded data.
     */
    protected void handleFOMAmmoState(Entity odmEntity, byte[] encodedData) {

        if (isDebugEnabled) {
            logger.debug("AbstractPlatformConverter handling FOM " + FOM_AMMO_STATE);
        }

        // Streams used for byte[] parsing
        XDRInputStream inXDR =
                new XDRInputStream(new ByteArrayInputStream(encodedData));

        try {
            List<BasicLoadSupply> supplies = null;
            boolean isAnythingChanged = false;

            if (odmEntity.getBasicLoad() != null) {
                supplies = odmEntity.getBasicLoad().getSupplies();
            }

            int listSize = inXDR.readInt();

            for (int i = 0; (i < listSize); ++i) {

                int munition = inXDR.readInt();
                int isDIS = inXDR.readInt();
                MunitionTypeEnum munitionType = null;
                String externalIdentifier = null;

                if (isDIS == 1) {
                    externalIdentifier = enumUtil.readDISEnum(inXDR);
                    munitionType = enumTranslator.getDISMunitionType(externalIdentifier);
                } else {
					munitionType = enumTranslator.getMunitionType(munition);
				}                

                int quantity = inXDR.readInt();
                double percent = inXDR.readDouble();

                if (munitionType != null) {

                    if (supplies == null) {
                        // Nothing in the basic load yet
                        isAnythingChanged = true;

                    } else {
                        // Is this incoming supply already in the basic load?
                        boolean matchFound = false;
                        for (BasicLoadSupply supply : supplies) {
                            if (supply.getSupplyType() == munitionType) {
                                matchFound = true;

                                // Yes, but has the quantity changed?
                                if (supply.getUsableQuantity() != quantity) {
                                    supply.setUsableQuantity((float)quantity);
                                    isAnythingChanged = true;
                                }
                            }
                        }
                        if (! matchFound) {
                            if (isInfoEnabled) {
                                logger.info("AbstractPlatformConverter Add ammo " + quantity);
                            }
                            SupplyThreshold threshold = 
                                SupplyThresholdFactory.getSupplyThreshold(0.75f, 0.50f, 0.25f);

                            BasicLoadSupply supply = 
                                new BasicLoadSupply(munitionType, 
                                                    (float)quantity,
                                                    (float)quantity, 
                                                    (float)quantity, 
                                                    SupplyThresholdEnum.GREEN,
                                                    threshold);
                            
                            supplies.add(supply);
                            isAnythingChanged = true;
                        }
                    }
                }

                if (isDebugEnabled) {
                    logger.debug("AbstractPlatformConverter.handleFOMAmmoState " +
                                 externalIdentifier + " " + 
                                 munitionType + " " + 
                                 quantity + " " + 
                                 percent);
                }
            }

            if ((isAnythingChanged) &&
                (supplies != null)) {
                loadBasicSupplies(odmEntity, supplies);
            }
        } catch (IOException ex) {
            logger.error(
                "IOException during conversion from " + FOM_AMMO_STATE, ex);

        } catch (Exception e) {
                logger.error("Exception caught " + FOM_AMMO_STATE,
                    e);
        } // end catch block
    }

    private void loadBasicSupplies(Entity entity, List<BasicLoadSupply> supplies) {
        List structList = new ArrayList();

        for (BasicLoadSupply supply : supplies) {
            BasicLoadMagicStruct struct = new BasicLoadMagicStruct(supply.getSupplyType());
            struct.setQuantity(supply.getUsableQuantity());
            struct.setCapacity(supply.getCapacity());
            struct.setAuthorizedAmount(supply.getAuthorizedAmount());
            struct.setSupplyThreshold(supply.getThreshold());
            structList.add(struct);            
        }

        magicServices.magicBasicLoadSupply(structList, entity);
    }

    /**
     * XDR decode a the FOM AvailableSupply attribute. 
     * 
     * @param odmEntity that may be dismounted.
     * @param encodedData byte[] XDR encoded data.
     */
    protected void handleFOMAvailableSupply(Entity odmEntity, byte[] encodedData) {

        if (isDebugEnabled) {
            logger.debug("AbstractPlatformConverter handling FOM " + FOM_AVAILABLE_SUPPLY);
        }

        // Streams used for byte[] parsing
        XDRInputStream inXDR =
                new XDRInputStream(new ByteArrayInputStream(encodedData));

        try {
            List<CargoMagicStruct> structList = new ArrayList<CargoMagicStruct>();
            boolean hasSupplies = false;

            int listSize = inXDR.readInt();
            if (isDebugEnabled) {
                logger.debug("AbstractPlatformConverter.handleFOMAvailableSupply " + listSize);
            }

            for (int i = 0; (i < listSize); ++i) {

                SupplyType supplyType = null;
                String externalIdentifier = enumUtil.readDISEnum(inXDR);

                //UnitOfIssue
                inXDR.readInt();

                // NumUnits
                double quantity = inXDR.readDouble();
                
                if (enumUtil.isFuelEnum(externalIdentifier)) {
                    supplyType = enumUtil.pol2Enum(externalIdentifier);
                    quantity /= 1000;
                } else {
                    supplyType = enumTranslator.getDISMunitionType(externalIdentifier);
                }
                
                if (supplyType != null) {
                    // Add this incoming supply to the structList.
                    addCargoToList(supplyType, quantity, structList);
                    hasSupplies = true;
                }

                if (isDebugEnabled) {
                    logger.debug("AbstractPlatformConverter.handleFOMAvailableSupply " +
                                 externalIdentifier + ":" + 
                                 supplyType + ":" + 
                                 quantity);
                }
            }

            if (hasSupplies) {

                boolean isAnythingChanged = false;
                if (odmEntity.getCargo() != null) {
                    isAnythingChanged = hasCargoChanged(odmEntity, structList);
                } else {
                    // Nothing in the cargo yet
                    isAnythingChanged = true;
                }
                
                if (isAnythingChanged) {
                    magicServices.magicCargoSupply(structList, odmEntity);
                }
            }

        } catch (IOException ex) {
            logger.error(
                "IOException during conversion from " + FOM_AVAILABLE_SUPPLY, ex);

        } catch (Exception e) {
                logger.error("Exception caught " + FOM_AVAILABLE_SUPPLY,
                    e);
        } // end catch block
    }

    private boolean hasCargoChanged(Entity odmEntity, 
                                    List<CargoMagicStruct> structList) {
        boolean isAnythingChanged = false;

        List cargoSupplies = odmEntity.getCargo().getSupplies();
        if (cargoSupplies != null) {

            // For each supply that is currently in the cargo, check
            // if is in the incoming list.
            Iterator suppliesIter = cargoSupplies.iterator();
            while (suppliesIter.hasNext()) {
                CargoSupply item = (CargoSupply)suppliesIter.next();
                SupplyType supply = item.getSupplyType();

                // Look through the incoming list for this supply.
                boolean isSupplyFound = false;
                for (CargoMagicStruct cargoStruct : structList) {
                    if (cargoStruct.getSupplyType() == supply) {
                        isSupplyFound = true;

                        if (cargoStruct.getQuantity() != item.getUsableQuantity()) {
                            isAnythingChanged = true;
                        }
                    }
                }
                if (! isSupplyFound) {
                    // This supply is currently in the cargo, but the
                    // incoming list doesn't have it, so set its
                    // quantity to 0.
                    addCargoToList(supply, 0, structList);
                    isAnythingChanged = true;
                }
            }

            // Now look through the incoming list for items which
            // aren't in the current cargo
            for (CargoMagicStruct cargoStruct : structList) {
                boolean isSupplyFound = false;
                suppliesIter = cargoSupplies.iterator();

                while (suppliesIter.hasNext()) {
                    CargoSupply item = (CargoSupply)suppliesIter.next();
                    if (cargoStruct.getSupplyType() == item.getSupplyType()) {
                        isSupplyFound = true;
                            
                        if (cargoStruct.getQuantity() != item.getUsableQuantity()) {
                            // This supply is in the incoming list and
                            // in the current cargo, but its quantity
                            // has changed.
                            isAnythingChanged = true;
                        }
                    }
                }
                if (! isSupplyFound) {
                    // This supply is in the incoming list, but is not
                    // in the current cargo
                    isAnythingChanged = true;
                }
            }
        }

        return isAnythingChanged;
    }

    private void addCargoToList(SupplyType supply, 
                                double quantity,
                                List<CargoMagicStruct> structList) {
        CargoMagicStruct struct = new CargoMagicStruct(supply);

        struct.setQuantity((float)quantity);
        struct.setDesiredQuantity((float)quantity);

        structList.add(struct);
    }

    /**
     * XDR decode a the FOM Fuel_Status attribute. 
     * 
     * @param odmEntity that may be dismounted.
     * @param encodedData byte[] XDR encoded data.
     */
    protected void handleFOMFuelStatus(Entity odmEntity, byte[] encodedData) {

        if (isDebugEnabled) {
            logger.debug("AbstractPlatformConverter handling FOM " + FOM_FUEL_STATUS);
        }

        // Streams used for byte[] parsing
        XDRInputStream inXDR =
                new XDRInputStream(new ByteArrayInputStream(encodedData));

        try {
            List<BasicLoadSupply> supplies = null;
            boolean isAnythingChanged = false;

            if (odmEntity.getBasicLoad() != null) {
                supplies = odmEntity.getBasicLoad().getSupplies();
            }

            int listSize = inXDR.readInt();

            for (int i = 0; (i < listSize); ++i) {
                int powerType = inXDR.readInt();
                double percent = inXDR.readDouble();
                double quantity = inXDR.readDouble() / 1000;
                BulkPOLSupplyTypeEnum fuelType = enumTranslator.getPowerSource(powerType);
                if (isInfoEnabled) {
                    logger.info("AbstractPlatformConverter Fuel " + 
                                powerType + " " + percent + " " + 
                                quantity + " " + fuelType);
                }

                if (supplies == null) {
                    // Nothing in the basic load yet
                    isAnythingChanged = true;
                    
                } else {
                    boolean matchFound = false;
                    for (BasicLoadSupply supply : supplies) {
                        if (supply.getSupplyType() == fuelType) {
                            matchFound = true;
                            if (isInfoEnabled) {
                                logger.info("AbstractPlatformConverter Update fuel " + quantity);
                            }
                            if (supply.getUsableQuantity() != quantity) {
                                supply.setUsableQuantity((float)quantity);
                                isAnythingChanged = true;
                            }
                        }
                    }
                    if (! matchFound) {

                        if (isInfoEnabled) {
                            logger.info("AbstractPlatformConverter Add fuel " + quantity);
                        }
                        SupplyThreshold threshold = 
                            SupplyThresholdFactory.getSupplyThreshold(0.75f, 0.50f, 0.25f);

                        BasicLoadSupply supply = 
                            new BasicLoadSupply(fuelType, 
                                                (float)quantity,
                                                (float)quantity, 
                                                (float)quantity, 
                                                SupplyThresholdEnum.GREEN,
                                                threshold);

                        supplies.add(supply);
                        isAnythingChanged = true;
                    }

                    if (isDebugEnabled) {
                        logger.debug("AbstractPlatformConverter.handleFOMFuelStatus " +
                                     powerType + " " + 
                                     fuelType + " " + 
                                     percent + " " + 
                                     quantity + " " + 
                                     matchFound);
                    }
                }
            }

            if ((isAnythingChanged) &&
                (supplies != null)) {
                loadBasicSupplies(odmEntity, supplies);
            }
        } catch (IOException ex) {

            logger.error(
                "IOException during conversion from " + FOM_FUEL_STATUS, ex);
        } catch (Exception e) {
                logger.error("Exception caught " + FOM_FUEL_STATUS,
                    e);
        } // end catch block
    }

    /**
     * XDR encode the data in <code>supplyFuelList</code> into a
     * FOMFuelStatusCDT attribute used by Platform objects.
     * 
     * @param odmEntity Entity entity whose fuel is to be published.
     * @param classHandle RTIHandle handle to the FOM class representative of
     *        odmEntity.
     * @param supplyFuelList the list of fuel to publish.
     * @param outputData Map store the XDR encoded data.
     */
    protected void handleODMFuel(Entity odmEntity,
            ObjectClassRTIHandle classHandle, List<BasicLoadSupply> supplyFuelList,
            Map<AttributeRTIHandle, byte[]> outputData) {

        BulkPOLSupplyTypeEnum fuelType;
        double fuelPercent;
        double fuelQuantity;
        double maxValue;

        if (isDebugEnabled) {

            logger.debug("AbstractPlatformConverter handling ODM " + ODM_BASIC_LOAD + " change");
        }

        // Streams used for byte[] creation
        ByteArrayOutputStream outByte = new ByteArrayOutputStream();
        XDROutputStream outXDR = new XDROutputStream(outByte);

        try {

            outXDR.writeInt(supplyFuelList.size());

            for (BasicLoadSupply currentSupply : supplyFuelList) {

                // Write FuelType (aka Power Source)
                fuelType =
                        (BulkPOLSupplyTypeEnum) BulkPOLSupplyTypeEnum
                        .valueOf((currentSupply.getSupplyType()).getName());
                int fuelEnum = enumTranslator.getPowerSource(fuelType).intValue();
                outXDR.writeInt(fuelEnum);

                // Store Fuel Qty to use to calculate fuel %
                fuelQuantity = currentSupply.getUsableQuantity();

                // Calculate and Write Fuel %
                maxValue = currentSupply.getCapacity();

                fuelPercent = (fuelQuantity / maxValue) * 100;
                outXDR.writeDouble(fuelPercent);

                // now Write Fuel Qty in liters (require multiply by 1000)
                outXDR.writeDouble(fuelQuantity * 1000.0);

                if (isDebugEnabled) {
                    logger.debug("AbstractPlatformConverter.handleFuel " +
                                 fuelType + " " + 
                                 fuelEnum + " " + 
                                 fuelQuantity + " " + 
                                 fuelPercent);
                }
            }

            setAttributeInClassHandle(classHandle, FOM_FUEL_STATUS,
                outByte.toByteArray(), outputData);
        } catch (IOException ex) {

            logger.error("IOException encountered when handling ODM "
                    + "basicLoad - fuel", ex);
        }
    }
}

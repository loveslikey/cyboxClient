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
package net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing;

import java.util.Set;

import net.onesaf.core.services.sim.interopmgt.hla.common.framework.amb.RTIAmbassadorException;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.ret.AbstractReturnMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.AttributeRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.amb.OneSAFHLA13RTIAmbassador;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.SubscribeObjectClassAttributesReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.rtihandle.HLA13ObjectClassRTIHandle;

/**
 * This class represents the SubscribeObjectClassAttributes RTI message.
 * 
 * @version $Revision$
 * @author $Author$
 */
public class SubscribeObjectClassAttributes extends AbstractOutgoingHLA13RTIMessage {

    private HLA13ObjectClassRTIHandle objectClassHandle;
    private Set<AttributeRTIHandle> attributeHandleSet;
    
    public SubscribeObjectClassAttributes() {
        objectClassHandle = null;
        attributeHandleSet = null;
    }
    
    public ObjectClassRTIHandle getObjectClassHandle() {
        return objectClassHandle;
    }

    public void setObjectClassHandle(ObjectClassRTIHandle objectClassHandle) {
        this.objectClassHandle = (HLA13ObjectClassRTIHandle) objectClassHandle;
    }

    public Set<AttributeRTIHandle> getAttributeHandleSet() {
        return attributeHandleSet;
    }

    public void setAttributeHandleSet(Set<AttributeRTIHandle> attributeHandleSet) {
        this.attributeHandleSet = attributeHandleSet;
    }

    @Override
    protected SubscribeObjectClassAttributesReturn createReturnMessage() {
        return new SubscribeObjectClassAttributesReturn();
    }

    @Override
    protected void populateReturnMessage(OneSAFHLA13RTIAmbassador rtiAmb,
            AbstractReturnMessage returnMsg) throws RTIAmbassadorException {
        
        rtiAmb.subscribeObjectClassAttributes(objectClassHandle.getHandle(),
            attributeHandleSet);
    }

}

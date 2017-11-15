#include "opfield.h"


void opfield::setspacederivative(int whichderivative)
{ 
    // Make sure a single space derivative is applied.
    if (spacederivative != 0 || kietaphiderivative != 0)
    {
        std::cout << "Error in 'opfield' object: cannot apply more than one space derivative to a field" << std::endl;
        abort();
    }
    spacederivative = whichderivative; 
}

void opfield::setkietaphiderivative(int whichderivative)
{ 
    // Make sure a single space derivative is applied.
    if (spacederivative != 0 || kietaphiderivative != 0)
    {
        std::cout << "Error in 'opfield' object: cannot apply more than one space derivative to a field" << std::endl;
        abort();
    }
    kietaphiderivative = whichderivative; 
}

void opfield::increasetimederivativeorder(int amount)
{    
    if (myfield->ismultiharmonic() == false)
    {
        std::cout << "Error in 'opfield' object: dt(field) is only supported on multiharmonic fields for the moment" << std::endl;
        abort();
    }
    
    timederivativeorder += amount; 

    if (timederivativeorder > 2)
    {
        std::cout << "Error in 'opfield' object: time derivative order cannot exceed 2" << std::endl;
        abort();
    }
}

bool opfield::isharmonicone(std::vector<int> disjregs) 
{ 
    std::vector<int> myharms = myfield->getharmonics(); 
    return (myharms.size() == 1 && myharms[0] == 1);
}

std::vector<std::vector<densematrix>> opfield::interpolate(int kietaphiderivative, elementselector& elemselect, std::vector<double>& evaluationcoordinates)
{
    if (timederivativeorder == 0 && spacederivative == 0)
        return myfield->interpolate(kietaphiderivative, formfunctioncomponent, elemselect, evaluationcoordinates);
    else
    {
        std::cout << "Error in 'opfield' object: expression provided for mesh deformation is invalid" << std::endl;
        std::cout << "Operation was:" << std::endl;
        this->print();
        abort();
    }
}

std::vector<std::vector<densematrix>> opfield::interpolate(elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform)
{
    // Get the value from the universe if available and reuse is enabled:
    if (reuse && universe::isreuseallowed)
    {
        int precomputedindex = universe::getindexofprecomputedvalue(shared_from_this());
        if (precomputedindex >= 0) { return universe::getprecomputed(precomputedindex); }
    }
    
    std::vector<std::vector<densematrix>> output;
    
    // In case there is no space derivative applied:
    if (spacederivative == 0 && kietaphiderivative == 0)
        output = myfield->interpolate(0, formfunctioncomponent, elemselect, evaluationcoordinates);
    else
    {
        if (kietaphiderivative != 0)
            output = myfield->interpolate(kietaphiderivative, formfunctioncomponent, elemselect, evaluationcoordinates);
        else
        {
            // Otherwise compute the x, y or z field derivative using ki, eta, 
            // phi derivatives in the reference element and invjac() terms.

            // Compute the Jacobian terms or reuse if available in the universe.
            shared_ptr<jacobian> myjac;
            if (universe::isreuseallowed && universe::computedjacobian != NULL)
                myjac = universe::computedjacobian;
            else
                myjac = shared_ptr<jacobian>(new jacobian(elemselect, evaluationcoordinates, meshdeform));

            if (universe::isreuseallowed)
                universe::computedjacobian = myjac;

            // Compute the required ki, eta and phi derivatives:
            std::vector<std::vector<densematrix>> dkiargmat, detaargmat, dphiargmat;

            // Get the element dimension in the selected elements:
            int elementdimension = elemselect.getelementdimension();  

                dkiargmat = myfield->interpolate(1, formfunctioncomponent, elemselect, evaluationcoordinates);
            if (elementdimension > 1)
                detaargmat = myfield->interpolate(2, formfunctioncomponent, elemselect, evaluationcoordinates);
            if (elementdimension > 2)
                dphiargmat = myfield->interpolate(3, formfunctioncomponent, elemselect, evaluationcoordinates);

            // Computing the x, y and z derivatives for all harmonics based on the
            // Jacobian matrix and the computed ki, eta and phi derivatives.
            // Skip the sin0 harmonic.
            for (int harm = 1; harm < dkiargmat.size(); harm++)
            {
                // Skip any non existent harmonic:
                if (dkiargmat[harm].size() == 0)
                    continue;

                    dkiargmat[harm][0].multiplyelementwise(myjac->getinvjac(spacederivative-1,0));
                if (elementdimension > 1)
                {
                    detaargmat[harm][0].multiplyelementwise(myjac->getinvjac(spacederivative-1,1));
                    dkiargmat[harm][0].add(detaargmat[harm][0]);
                }
                if (elementdimension > 2)
                {
                    dphiargmat[harm][0].multiplyelementwise(myjac->getinvjac(spacederivative-1,2));
                    dkiargmat[harm][0].add(dphiargmat[harm][0]);
                }
            }
            output = dkiargmat;
        }
    }
    
    if (timederivativeorder == 0)
    {
        if (reuse && universe::isreuseallowed)
            universe::setprecomputed(shared_from_this(), output);
        
        return output;	
    }
    
    // Time derivative for multiharmonic fields only.
    output = harmonic::timederivative(timederivativeorder, output);
    
    if (reuse && universe::isreuseallowed)
        universe::setprecomputed(shared_from_this(), output);
    return output;
}

densematrix opfield::multiharmonicinterpolate(int numtimeevals, elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform)
{
    // Get the value from the universe if available and reuse is enabled:
    if (reuse && universe::isreuseallowed)
    {
        int precomputedindex = universe::getindexofprecomputedvaluefft(shared_from_this());
        if (precomputedindex >= 0) { return universe::getprecomputedfft(precomputedindex); }
    }
    
    std::vector<std::vector<densematrix>> interpolatedfield = interpolate(elemselect, evaluationcoordinates, meshdeform);
    // Compute at 'numtimevals' instants in time the multiharmonic field:
    densematrix output = myfft::inversefft(interpolatedfield, numtimeevals, elemselect.countinselection(), evaluationcoordinates.size()/3);
    
    if (reuse && universe::isreuseallowed)
        universe::setprecomputedfft(shared_from_this(), output);
    return output;
}

bool opfield::isvalueorientationdependent(std::vector<int> disjregs)
{
    if (myfield->gettypename() == "x" || myfield->gettypename() == "y" || myfield->gettypename() == "z")
        return false;
    
    for (int i = 0; i < disjregs.size(); i++)
    {
        int elementtypenumber = (universe::mymesh->getdisjointregions())->getelementtypenumber(disjregs[i]);
        std::shared_ptr<hierarchicalformfunction> myformfunction = selector::select(elementtypenumber, myfield->gettypename());
        if ( myformfunction->isorientationdependent(myfield->getinterpolationorder(disjregs[i])) )
            return true;
    }
    return false;
}

std::shared_ptr<operation> opfield::copy(void)
{
    std::shared_ptr<opfield> op(new opfield(myfield));
    *op = *this;
    return op;
}

void opfield::print(void)
{
    for (int i = 0; i < timederivativeorder; i++)
        std::cout << "dt";
    
    std::vector<std::string> nonedxdydz = {"","dx","dy","dz"};
    std::vector<std::string> nonecompxcompycompz = {"compx","compy","compz"};

    std::cout << nonedxdydz[spacederivative];
    // For fields without subfields:
    if (fieldcomponent == -1)
    {
        if (myfield->countformfunctioncomponents() > 1)
            std::cout << nonecompxcompycompz[formfunctioncomponent];
    }
    else
        std::cout << nonecompxcompycompz[fieldcomponent];
    
    myfield->print();
}

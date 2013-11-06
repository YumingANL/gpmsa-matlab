/*
 *  LAmodelMCMC.h
 *  gpmsa
 *
 *  Created by gatt on 4/2/08.
 *  Copyright 2008 LANL. All rights reserved.
 *
 */
#include <iostream>
#include <stdio.h>
#include <math.h>

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>


class LAmodelMCMC {
  public:
    // inputs
	  LAmodel *mod, *m2;
	  struct MCMCparams {
	    Prior *prior;
	    double width;
	  };
	  int n,m,p,q,pu,pv;
      MCMCparams theta, betaU, betaV, lamVz, lamUz, lamWs, lamWOs, lamOs;
	  int verbose;

 LAmodelMCMC (LAmodel *min, FILE *fp) { 
    verbose=0;
    mod=min;
	n=mod->n; m=mod->m; p=mod->p; q=mod->q; pu=mod->pu; pv=mod->pv;
	if (n>0) readPriorMCMC(fp,&theta,1);
	if (n>0) readPriorMCMC(fp,&betaV,4);
	readPriorMCMC(fp,&betaU,4);
	if (n>0) readPriorMCMC(fp,&lamVz,2);
	readPriorMCMC(fp,&lamUz,2);
	readPriorMCMC(fp,&lamWs,2);
	readPriorMCMC(fp,&lamWOs,2);
	if (n>0) readPriorMCMC(fp,&lamOs,2);

    if (n>0) {
	  if (verbose) printf("theta  %8.4f %8.4f %8.4f %8.4f %8.4f \n",theta.width,theta.prior->lBound,theta.prior->uBound,
             theta.prior->params[0],theta.prior->params[1]);
	  if (verbose) printf("betaV   %8.4f %8.4f %8.4f %8.4f %8.4f \n",betaV.width,betaV.prior->lBound,betaV.prior->uBound,
             betaV.prior->params[0],betaV.prior->params[1]);
	  if (verbose) printf("lamVz  %8.4f %8.4f %8.4f %8.4f %8.4f \n",lamVz.width,lamVz.prior->lBound,lamVz.prior->uBound,
             lamVz.prior->params[0],lamVz.prior->params[1]);
	  if (verbose) printf("lamOs  %8.4f %8.4f %8.4f %8.4f %8.4f \n",lamOs.width,lamOs.prior->lBound,lamOs.prior->uBound,
             lamOs.prior->params[0],lamOs.prior->params[1]);
    }
	if (verbose) printf("betaU   %8.4f %8.4f %8.4f %8.4f %8.4f \n",betaU.width,betaU.prior->lBound,betaU.prior->uBound,
             betaU.prior->params[0],betaU.prior->params[1]);
	if (verbose) printf("lamUz  %8.4f %8.4f %8.4f %8.4f %8.4f \n",lamUz.width,lamUz.prior->lBound,lamUz.prior->uBound,
             lamUz.prior->params[0],lamUz.prior->params[1]);
	if (verbose) printf("lamWs  %8.4f %8.4f %8.4f %8.4f %8.4f \n",lamWs.width,lamWs.prior->lBound,lamWs.prior->uBound,
             lamWs.prior->params[0],lamWs.prior->params[1]);
	if (verbose) printf("lamWOs %8.4f %8.4f %8.4f %8.4f %8.4f \n",lamWOs.width,lamWOs.prior->lBound,lamWOs.prior->uBound,
             lamWOs.prior->params[0],lamWOs.prior->params[1]);
	
 }
 ~LAmodelMCMC() {
   if (n>0) delete theta.prior; 
   if (n>0) delete betaV.prior; 
   delete betaU.prior; 
   if (n>0) delete lamVz.prior; 
   delete lamUz.prior; 
   delete lamWs.prior; 
   delete lamWOs.prior; 
   if (n>0) delete lamOs.prior; 
 }
 void readPriorMCMC(FILE *fp, MCMCparams *mp, int ptype) {
   float fin,fin1,fin2,fin3,fin4;
   fscanf(fp,"%f",&fin); mp->width=fin;
   fscanf(fp,"%f",&fin3); 
   fscanf(fp,"%f",&fin4); 
   fscanf(fp,"%f",&fin1); 
   fscanf(fp,"%f",&fin2); 
   mp->prior=new Prior(fin1,fin2,ptype,fin3,fin4);   
 }
 
 void writePvalsHeader(FILE *fp) {
   fprintf(fp,"%d %d %d %d %d %d\n",n,m,p,q,pv,pu);
 }
 void writePvals(FILE *fp) {
	  if (n>0) {
	    for (int ii=0;ii<q;ii++) { fprintf(fp,"%f ",gsl_vector_get(mod->pp->theta,ii)); }
	      fprintf(fp,"\n");
	    for (int ii=0; ii<1; ii++) for (int jj=0; jj<p; jj++) 
	      { fprintf(fp,"%f ",gsl_vector_get(mod->pp->betaV[ii],jj)); }
	      fprintf(fp,"\n");
	    for (int ii=0; ii<1; ii++) { fprintf(fp,"%f ",gsl_vector_get(mod->pp->lamVz,ii)); }
	      fprintf(fp,"\n");
	    fprintf(fp,"%f\n",mod->pp->lamOs);
	  }
	  for (int ii=0; ii<pu; ii++) for (int jj=0; jj<(p+q); jj++) 
	    { fprintf(fp,"%f ",gsl_vector_get(mod->pp->betaU[ii],jj)); }
	    fprintf(fp,"\n");
	  for (int ii=0; ii<pu; ii++) { fprintf(fp,"%f ",gsl_vector_get(mod->pp->lamUz,ii)); }
	    fprintf(fp,"\n");
	  for (int ii=0; ii<pu; ii++) { fprintf(fp,"%f ",gsl_vector_get(mod->pp->lamWs,ii)); }
	    fprintf(fp,"\n");
	  fprintf(fp,"%f\n",mod->pp->lamWOs);
	  fprintf(fp,"%f\n",mod->getLogLik());
	  fprintf(fp,"%f\n",computeLogPrior(0));
	  fprintf(fp,"\n");
 }
 
 void sampleRound() {
   double newVal, aCorr;
   
   if (n>0) {
     // Sample Theta
     for (int ii=0; ii<q; ii++) {
	   newVal=gsl_vector_get(mod->pp->theta,ii) + (urand() - 0.5) * theta.width;
	   if (verbose) printf("Theta %2d: Updating with value %f\n",ii,newVal);
       if (theta.prior->withinBounds(newVal)) {
  	     m2=new LAmodel(mod);
	     m2->updateTheta(ii,newVal);
	     if (accept(m2->computeLogLik(), theta.prior->computeLogPrior(gsl_vector_get(m2->pp->theta,ii)),
	  			    mod->getLogLik(),    theta.prior->computeLogPrior(gsl_vector_get(mod->pp->theta,ii)), 1) ) {
		   delete mod;
	       mod=m2;
	     } else {
	       delete m2;
	     }
	   }
     }
     if (verbose) printf("Completed sampling theta, logPrior=%f\n",computeLogPrior(0));

     // Sample betaV
     for (int ii=0; ii<1; ii++) 
       for (int jj=0; jj<p; jj++) {
	     newVal=exp(-0.25*gsl_vector_get(mod->pp->betaV[ii],jj)) + (urand() - 0.5) * betaV.width;
	     newVal=-4*log(newVal);
	     if (verbose) printf("betaV %2d-%2d: Updating with value %f\n",ii,jj,newVal);
	     if (betaV.prior->withinBounds(newVal)) {
   	       m2=new LAmodel(mod);
	       m2->updateBetaV(ii,jj,newVal);
	       if (accept(m2->computeLogLik(), betaV.prior->computeLogPrior(gsl_vector_get(m2->pp->betaV[ii],jj)),
			  	      mod->getLogLik(),    betaV.prior->computeLogPrior(gsl_vector_get(mod->pp->betaV[ii],jj)), 1) ) {
		   delete mod;
	       mod=m2;
	     } else {
	       delete m2;
	     }
	   }
     }
     if (verbose) printf("Completed sampling betaV, logPrior=%f\n",computeLogPrior(0));
     // Sample lamVz
     for (int ii=0; ii<1; ii++) {
	   aCorr=chooseVal(gsl_vector_get(mod->pp->lamVz,ii),&newVal);
	   if (verbose) printf("lamVz %2d: Updating with value %f\n",ii,newVal);
	   if (aCorr!=0) {
	     if (lamVz.prior->withinBounds(newVal)) {
 	       m2=new LAmodel(mod);
	       m2->updateLamVz(ii,newVal);
	       if (accept(m2->computeLogLik(), lamVz.prior->computeLogPrior(gsl_vector_get(m2->pp->lamVz,ii)),
   	 	  	          mod->getLogLik(),    lamVz.prior->computeLogPrior(gsl_vector_get(mod->pp->lamVz,ii)), aCorr) ) {
		     delete mod;
	         mod=m2;
	       } else {
	         delete m2;
	       }
	     }
       }
     }
     if (verbose) printf("Completed sampling lamVz, logPrior=%f\n",computeLogPrior(0));
	 
     // Sample lamOs
	   aCorr=chooseVal(mod->pp->lamOs,&newVal);
	   if (verbose) printf("lamOs: Updating with value %f\n",newVal);
	   if (aCorr!=0) {
	     if (lamOs.prior->withinBounds(newVal)) {
	       m2=new LAmodel(mod);
           m2->updateLamOs(newVal);
	       if (accept(m2->computeLogLik(), lamOs.prior->computeLogPrior(m2->pp->lamOs),
   		 	          mod->getLogLik(),    lamOs.prior->computeLogPrior(mod->pp->lamOs), aCorr) ) {
		     delete mod;
	         mod=m2;
	       } else {
	         delete m2;
	       }
	     }
       }
     if (verbose) printf("Completed sampling lamOs, logPrior=%f\n",computeLogPrior(0)); 	 
   }

   // Sample betaU
   for (int ii=0; ii<pu; ii++) 
     for (int jj=0; jj<(p+q); jj++) {
	   newVal=exp(-0.25*gsl_vector_get(mod->pp->betaU[ii],jj)) + (urand() - 0.5) * betaU.width;
	   newVal=-4*log(newVal);
	   if (verbose) printf("betaU %2d-%2d: Updating with value %f\n",ii,jj,newVal);
	   if (betaU.prior->withinBounds(newVal)) {
  	     m2=new LAmodel(mod);
	     m2->updateBetaU(ii,jj,newVal);
	     if (accept(m2->computeLogLik(), betaU.prior->computeLogPrior(gsl_vector_get(m2->pp->betaU[ii],jj)),
	                mod->getLogLik(),    betaU.prior->computeLogPrior(gsl_vector_get(mod->pp->betaU[ii],jj)), 1) ) {
		 delete mod;
	     mod=m2;
	   } else {
	     delete m2;
	   }
	 }
   }
   if (verbose) printf("Completed sampling betaU, logPrior=%f\n",computeLogPrior(0));

   // Sample lamUz
   for (int ii=0; ii<pu; ii++) {
	 aCorr=chooseVal(gsl_vector_get(mod->pp->lamUz,ii),&newVal);
	 if (verbose) printf("lamUz %2d: Updating with value %f\n",ii,newVal);
	 if (aCorr!=0) {
	   if (lamUz.prior->withinBounds(newVal)) {
	     m2=new LAmodel(mod);
	     m2->updateLamUz(ii,newVal);
	     if (accept(m2->computeLogLik(), lamUz.prior->computeLogPrior(gsl_vector_get(m2->pp->lamUz,ii)),
   			        mod->getLogLik(),    lamUz.prior->computeLogPrior(gsl_vector_get(mod->pp->lamUz,ii)), aCorr) ) {
		   delete mod;
	       mod=m2;
	     } else {
	       delete m2;
	     }
	   }
     }
   }
   if (verbose) printf("Completed sampling lamUz, logPrior=%f\n",computeLogPrior(0));

   // Sample lamWs
   for (int ii=0; ii<pu; ii++) {
	 aCorr=chooseVal(gsl_vector_get(mod->pp->lamWs,ii),&newVal);
	 if (verbose) printf("lamWs %2d: Updating with value %f\n",ii,newVal);
	 if (aCorr!=0) {
	   if (lamWs.prior->withinBounds(newVal)) {
  	     m2=new LAmodel(mod);
	     m2->updateLamWs(ii,newVal);
	     if (accept(m2->computeLogLik(), lamWs.prior->computeLogPrior(gsl_vector_get(m2->pp->lamWs,ii)),
   		  	        mod->getLogLik(),    lamWs.prior->computeLogPrior(gsl_vector_get(mod->pp->lamWs,ii)), aCorr) ) {
		   delete mod;
	       mod=m2;
	     } else {
	       delete m2;
	     }
	   }
     }
   }
   if (verbose) printf("Completed sampling lamWs, logPrior=%f\n",computeLogPrior(0));
   // Sample lamWOs
	 aCorr=chooseVal(mod->pp->lamWOs,&newVal);
	 if (verbose) printf("lamWOs: Updating with value %f\n",newVal);
	 if (aCorr!=0) {
	   if (lamWOs.prior->withinBounds(newVal)) {
	     m2=new LAmodel(mod);
	     m2->updateLamWOs(newVal);
	     if (accept(m2->computeLogLik(), lamWOs.prior->computeLogPrior(m2->pp->lamWOs),
   		  	        mod->getLogLik(),    lamWOs.prior->computeLogPrior(mod->pp->lamWOs), aCorr) ) {
		   delete mod;
	       mod=m2;
	     } else {
	       delete m2;
	     }
	   }
     }
   if (verbose) printf("Completed sampling lamWOs, logPrior=%f\n",computeLogPrior(0));   // Sample lamOs

}

 double urand() {
   return rand()*1.0/RAND_MAX;
 }

 double chooseVal(double cval, double *dval) { // choose a new value in an interval around the current value
   // select the interval, and draw a new value.
   double w=cval/3; if (w<1) w=1;
   *dval=cval + (urand()*2-1)*w;
   // do a correction, which depends on the old and new interval
   double w1=*dval/3; if (w1<1) w1=1;
   double acorr;
   if (cval > (*dval+w1))
     acorr=0;
   else
     acorr=w/w1;
   //printf("cval=%10.4f, dval=%10.4f, acorr=%10.4f\n",cval,*dval,acorr);
   return acorr;
 }

 int accept(double newLik, double newPrior, double oldLik, double oldPrior, double correction) {
   double testVal;
   testVal=log(urand());
   if (verbose)
      printf("MCMC (%10.4f<%10.4f)? to newLik=%10.4f,newPrior=%10.4f to oldLik=%10.4f,oldPrior=%10.4f, corr=%10.4f ... ",
	         testVal, newLik+newPrior-oldLik-oldPrior+log(correction), newLik, newPrior, oldLik, oldPrior, correction);
   if (testVal < ( (newLik + newPrior) - (oldLik + oldPrior) + log(correction) ) ) {
      if (verbose) printf("accept.\n");
      return 1;
	}
	else {
      if (verbose) printf("reject.\n");
	  return 0;
	}
 }
 
 double computeLogPrior() {return computeLogPrior(0);}
 double computeLogPrior(int verbose) { 
   double lp=0;
   if (n>0) { 
	 lp+=theta.prior->computeLogPrior(mod->pp->theta);    if (verbose) printf("theta %f\n",lp);
	 lp+=betaV.prior->computeLogPrior(mod->pp->betaV,1);  if (verbose) printf("betaV  %f\n",lp);
     lp+=lamOs.prior->computeLogPrior(mod->pp->lamOs);    if (verbose) printf("lamOs %f\n",lp);
	 lp+=lamVz.prior->computeLogPrior(mod->pp->lamVz);    if (verbose) printf("lamVz %f\n",lp);
    }
   lp+=betaU.prior->computeLogPrior(mod->pp->betaU,pu); if (verbose) printf("betaU %f\n",lp);
   lp+=lamUz.prior->computeLogPrior(mod->pp->lamUz);    if (verbose) printf("lamUz %f\n",lp);
   lp+=lamWs.prior->computeLogPrior(mod->pp->lamWs);    if (verbose) printf("lamWs %f\n",lp);
   lp+=lamWOs.prior->computeLogPrior(mod->pp->lamWOs);  if (verbose) printf("lamWOs %f\n",lp);
   
   return lp;
 }
};



/** \file
 *
 * $Date:  03/03/2006 18:25:28 CET $
 * $Revision: 1.0 $
 * \author Stefano Lacaprara - INFN Legnaro <stefano.lacaprara@pd.infn.it>
 */

/* This Class Header */
#include "RecoLocalMuon/DTSegment/src/DTSegmentCand.h"

/* Collaborating Class Header */

/* C++ Headers */

/* ====================================================================== */
double DTSegmentCand::chi2max=20.; // to be tuned!!
unsigned int DTSegmentCand::nHitsMin=3; // to be tuned!!

/// Constructor
DTSegmentCand::DTSegmentCand(AssPointCont& hits,
                             const DTSuperLayer* sl) :
theSL(sl), theChi2(-1.) , theHits(hits){

}

DTSegmentCand::DTSegmentCand(AssPointCont hits,
                             LocalPoint& position,
                             LocalVector& direction,
                             double chi2,
                             AlgebraicSymMatrix covMat,
                             const DTSuperLayer* sl) :
theSL(sl), thePosition(position), theDirection(direction), theChi2(chi2),
  theCovMatrix( covMat), theHits(hits) {
}

/// Destructor
DTSegmentCand::~DTSegmentCand() {
}

/* Operations */ 
bool DTSegmentCand::operator==(const DTSegmentCand& seg){
  static const double epsilon=0.00001;
  if (nHits()!=seg.nHits()) return false;
  if (fabs(chi2()-seg.chi2())>epsilon) return false;
  if (fabs(position().x()-seg.position().x())>epsilon ||
      fabs(position().y()-seg.position().y())>epsilon ||
      fabs(position().z()-seg.position().z())>epsilon) return false;
  if (fabs(direction().x()-seg.direction().x())>epsilon ||
      fabs(direction().y()-seg.direction().y())>epsilon ||
      fabs(direction().z()-seg.direction().z())>epsilon) return false;
  return true;
}

bool DTSegmentCand::operator<(const DTSegmentCand& seg){
  if (nHits()==seg.nHits()) return (chi2()>seg.chi2());
  return (nHits()<seg.nHits());
}

void DTSegmentCand::add(DTHitPairForFit* hit, DTEnums::DTCellSide code) {
  theHits.insert(AssPoint(hit,code));
}

void DTSegmentCand::removeHit(AssPoint badHit) {
  theHits.erase(badHit);
}

int DTSegmentCand::nSharedHitPairs(const DTSegmentCand& seg) const{
  int result=0;
  for (AssPointCont::const_iterator hit=hits().begin(); 
       hit!=hits().end() ; ++hit) {
    for (AssPointCont::const_iterator hit2=seg.hits().begin();
         hit2!=seg.hits().end() ; ++hit2) {
      if ((*(*hit).first)==(*(*hit2).first)) {
        ++result;
        continue;
      }
    }
  }
  return result;
}

DTSegmentCand::AssPointCont
DTSegmentCand::conflictingHitPairs(const DTSegmentCand& seg) const{
  AssPointCont result;
  if (nSharedHitPairs(seg)==0) return result;
  for (AssPointCont::const_iterator hit=hits().begin(); 
       hit!=hits().end() ; ++hit) {
    for (AssPointCont::const_iterator hit2=seg.hits().begin();
         hit2!=seg.hits().end() ; ++hit2) {
      if ((*(*hit).first)==(*(*hit2).first) &&
          (*hit).second!=(*hit2).second) {
        result.insert(*hit);
        continue;
      }
    }
  }
  return result;
}

bool DTSegmentCand::good() const {
  return nHits()>=nHitsMin && chi2()/NDOF() < chi2max ;
}

int DTSegmentCand::nLayers() const {
  // TODO
  return 0;
}

bool DTSegmentCand::AssPointLessZ::operator()(const AssPoint& pt1, 
                                              const AssPoint& pt2) const {
  return *(pt1.first) < *(pt2.first);
}

std::ostream& operator<<(std::ostream& out, const DTSegmentCand& seg) {
  out <<  " chi2/nHits: " << seg.chi2() << "/" << seg.nHits() ;
  return out;
}

std::ostream& operator<<(std::ostream& out, const DTSegmentCand::AssPoint& hit) {
  // out << "Hits " << (hit.first)->localPosition(DTEnums::Left) <<
  //     " " << hit.second  << " Lay " << (hit.first)->layerNumber() << endl;
  return out;
}


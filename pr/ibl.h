#ifndef IBL_H
#define IBL_H

/* Common infrastructure of instance based learning algorithms
 * (IBL).
 */

#include <memory>
#include "pr/data_set.h"
#include "pr/distance.h"
#include "pr/nearest_neighbor.h"

struct ibl {
    virtual void train( const DataSet& ) = 0;
    virtual int hit_count() const = 0;
    virtual int miss_count() const = 0;
    virtual const DataSet & conceptual_descriptor() const = 0;
    virtual const NearestNeighbor & nearest_neighbor() const = 0;

    virtual ~ibl() = default;
};

class ibl1 : public ibl {
    std::unique_ptr<NearestNeighbor> nn;
    int hit = 0;
    int miss = 0;

public:
    virtual void train( const DataSet& ) override;
    virtual int hit_count() const override;
    virtual int miss_count() const override;
    virtual const DataSet & conceptual_descriptor() const override;
    virtual const NearestNeighbor & nearest_neighbor() const override;
};

class ibl2 : public ibl {
    std::unique_ptr<NearestNeighbor> nn;
    int hit = 0;
    int miss = 0;

public:
    virtual void train( const DataSet& ) override;
    virtual int hit_count() const override;
    virtual int miss_count() const override;
    virtual const DataSet & conceptual_descriptor() const override;
    virtual const NearestNeighbor & nearest_neighbor() const override;
};

#endif // IBL_H

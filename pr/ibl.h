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
};

class ibl3 : public ibl {
    DataSet _conceptual_descriptor;
    int hit = 0;
    int miss = 0;
    double accepting_threshold;
    double rejecting_threshold;

protected:
    /* The purpose of these methods is to ease the implementation of IBL 4 and 5.
     * do_distance is called several times per classification and queries
     * the distance between two data entries.
     *
     * The default implementation is to return the euclidean distance.
     */
    virtual double do_distance( const DataEntry &, const DataEntry & ) const;

    /* This function is called after every classification.
     * The first parameter is the just-classified entry.
     * The second parameter is the closest acceptable entry present in the dataset.
     *
     * The default implementation is a no-op.
     */
    virtual void do_update_weights( const DataEntry &, const DataEntry & );

public:
    ibl3( double accepting_threshold = 0.9, double rejecting_threshold = 0.75 );
    virtual void train( const DataSet& ) override;
    virtual int hit_count() const override;
    virtual int miss_count() const override;
    virtual const DataSet & conceptual_descriptor() const override;
};

#endif // IBL_H

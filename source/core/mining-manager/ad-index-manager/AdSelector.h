#ifndef SF1_AD_SELECTOR_H_
#define SF1_AD_SELECTOR_H_

#include "AdClickPredictor.h"
#include <util/singleton.h>
#include <boost/lexical_cast.hpp>
#include <common/PropSharedLockSet.h>
#include <common/NumericPropertyTable.h>
#include <ir/id_manager/IDManager.h>
#include <search-manager/NumericPropertyTableBuilder.h>
#include <boost/unordered_map.hpp>
#include <bitset>
#include <boost/thread.hpp>
#include <boost/random.hpp>

namespace sf1r
{

class AdClickPredictor;
class DocumentManager;
namespace faceted
{
    class GroupManager;
}

class AdSelector
{
public:
    static AdSelector* get()
    {
        return izenelib::util::Singleton<AdSelector>::get();
    }
    enum SegType
    {
        UserSeg,
        AdSeg,
        TotalSeg
    };

    // value may have multi possible values, each elem in the vector stand for
    //  the each value.
    typedef std::vector<std::string> FeatureValueT;
    typedef std::vector<std::pair<std::string, std::string> > FeatureT;
    typedef std::map<std::string, FeatureValueT > FeatureMapT;
    AdSelector();
    ~AdSelector();

    void init(const std::string& res_path,
        const std::string& segments_data_path,
        AdClickPredictor* pad_predictor,
        faceted::GroupManager* grp_mgr);
    void stop();

    bool selectFromRecommend(const FeatureT& user_info, std::size_t max_return,
        std::vector<docid_t>& recommended_doclist,
        std::vector<double>& score_list);

    bool select(const FeatureT& user_info,
        //const std::vector<FeatureMapT>& ad_feature_list, 
        std::size_t max_select,
        std::vector<docid_t>& ad_doclist,
        std::vector<double>& score_list,
        std::size_t max_ret_num,
        PropSharedLockSet& propSharedLockSet);

    bool selectForTest(const FeatureT& user_info,
        std::size_t max_select,
        std::vector<docid_t>& ad_doclist,
        std::vector<double>& score_list,
        std::size_t max_ret_num);

    void updateClicked(docid_t ad_id);
    void updateSegments(const FeatureT& segments, SegType type);
    void updateSegments(const std::string& segment_name, const std::set<std::string>& segments, SegType type);
    void load();
    void save();
    void getDefaultFeatures(FeatureMapT& feature_name_list, SegType type);
    void getAdSegmentStrList(docid_t ad_id, std::vector<std::string>& retstr_list);
    void updateAdSegmentStr(docid_t ad_docid, const FeatureMapT& ad_feature, std::vector<uint32_t>& segids);
    void updateAdSegmentStr(const std::vector<docid_t>& ad_doclist, const std::vector<FeatureMapT>& ad_feature_list);
    void miningAdSegmentStr(docid_t startid, docid_t endid);

private:

    static const int MAX_AD_DOCID = 1024*1024*1024;

    void loadDef(const std::string& file, FeatureMapT& def_features,
        std::map<std::string, std::size_t>& init_counter);
    void updateFunc();
    void selectByRandSelectPolicy(std::size_t max_unclicked_retnum, std::vector<docid_t>& unclicked_doclist);
    void computeHistoryCTR();
    double getHistoryCTR(const std::vector<std::string>& all_fullkey);
    void expandSegmentStr(std::vector<std::string>& seg_str_list, const std::vector<uint32_t>& ad_segid_list);
    void expandSegmentStr(std::vector<std::string>& seg_str_list, const FeatureMapT& feature_list);
    void getUserSegmentStr(std::vector<std::string>& user_seg_str_list, const FeatureT& user_info);
    void getAllPossibleSegStr(const FeatureMapT& segments,
        std::map<std::string, std::size_t> segments_counter,
        std::vector<std::pair<std::string, FeatureT> >& all_keys);

    typedef izenelib::ir::idmanager::_IDManager<std::string, std::string, uint32_t,
            izenelib::util::ReadWriteLock,
            izenelib::ir::idmanager::EmptyWildcardQueryHandler<std::string, uint32_t>,
            izenelib::ir::idmanager::HashIDGenerator<std::string, uint32_t>,
            izenelib::ir::idmanager::EmptyIDStorage<std::string, uint32_t>,
            izenelib::ir::idmanager::UniqueIDGenerator<std::string, uint32_t>,
            izenelib::ir::idmanager::EmptyIDStorage<std::string, uint32_t> > AdSegIDManager;

    faceted::GroupManager* groupManager_;
    std::string res_path_;
    std::string segments_data_path_;
    AdClickPredictor* ad_click_predictor_;
    boost::unordered_map<std::string, double> history_ctr_data_;
    std::bitset<MAX_AD_DOCID>  clicked_ads_;
    std::vector<FeatureMapT>  all_segments_;
    FeatureMapT  updated_segments_[TotalSeg];
    boost::mutex segment_mutex_;
    std::map<std::string, std::size_t> init_counter_[TotalSeg];
    FeatureMapT default_full_features_[TotalSeg];
    boost::thread  ctr_update_thread_;
    typedef boost::random::mt19937 EngineT;
    typedef boost::random::uniform_int_distribution<uint32_t> DistributionT;
    EngineT random_eng_;
    boost::random::variate_generator<EngineT&, DistributionT>  random_gen_;
    bool need_refresh_;
    std::vector< std::vector<uint32_t> >  ad_segid_data_;
    boost::shared_ptr<AdSegIDManager> ad_segid_mgr_;
    Lux::IO::Array  ad_segid_str_data_;
    boost::shared_mutex ad_segid_mutex_;
};

} //namespace sf1r

#endif

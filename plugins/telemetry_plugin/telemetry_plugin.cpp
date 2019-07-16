/**
 *  @file
 *  @copyright defined in haya/LICENSE
 */
#include <eosio/telemetry_plugin/telemetry_plugin.hpp>
#include <fc/exception/exception.hpp>
#include <eosio/chain/plugin_interface.hpp>
#include <prometheus/exposer.h>

#define LATENCY_HISTOGRAM_KEYPOINTS \
    {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 15000, 20000, 180000}


namespace eosio {
    using namespace chain::plugin_interface;
    using namespace prometheus;

    static appbase::abstract_plugin &_telemetry_plugin = app().register_plugin<telemetry_plugin>();

    class telemetry_plugin_impl {
    private:
        channels::accepted_block::channel_type::handle _on_accepted_block_handle;
        channels::irreversible_block::channel_type::handle _on_irreversible_block_handle;

        std::unique_ptr<Exposer> exposer;
        std::shared_ptr<Registry> registry;

        std::unique_ptr<Counter> accepted_trx_count;
        std::unique_ptr<Histogram> irreversible_latency_hist;
        std::unique_ptr<Gauge> last_irreversible_latency;

        void start_server() {
            exposer = std::make_unique<Exposer>(endpoint, uri, threads);
        }

        void add_event_handlers() {
            _on_accepted_block_handle = app().get_channel<channels::accepted_block>()
                    .subscribe([this](block_state_ptr s) {
                        accepted_trx_count->Increment(s.get()->trxs.size());
                    });

            _on_irreversible_block_handle = app().get_channel<channels::irreversible_block>()
                    .subscribe([this](block_state_ptr s) {
                        fc::microseconds latency = fc::time_point::now() - s.get()->header.timestamp.to_time_point();
                        int64_t latency_millis = latency.count() / 1000;
                        last_irreversible_latency->Set(latency_millis);
                        irreversible_latency_hist->Observe(latency_millis);
                    });
        }

        void add_metrics() {
            accepted_trx_count.reset(
                    &BuildCounter()
                            .Name("accepted_trx_total")
                            .Help("Total amount of transactions accepted")
                            .Register(*registry)
                            .Add({}));


            irreversible_latency_hist.reset(
                    &BuildHistogram()
                            .Name("irreversible_latency")
                            .Help("The latency of irreversible blocks")
                            .Register(*registry)
                            .Add({},
                                 Histogram::BucketBoundaries{
                                         LATENCY_HISTOGRAM_KEYPOINTS
                                 })
            );

            last_irreversible_latency.reset(
                    &BuildGauge()
                            .Name("last_irreversible_latency")
                            .Help("The latency of the last irreversible blocks")
                            .Register(*registry)
                            .Add({})
            );


            exposer->RegisterCollectable(std::weak_ptr<Registry>(registry));
        }

    public:
        std::string endpoint;
        std::string uri;
        size_t threads{};
        map<string, unique_ptr<Counter>> counter_map;
        map<string, unique_ptr<Gauge>> gauge_map;
        map<string, unique_ptr<Histogram>> histogram_map;

        telemetry_plugin_impl() {
            registry = std::make_shared<Registry>();
        }

        void initialize() {
            start_server();
            add_metrics();
            add_event_handlers();
        }

        void add_counter(const std::string& name) {
            counter_map[name].reset(&BuildCounter()
                    .Name(name)
                    .Register(*registry)
                    .Add({}));

        }

        void update_counter(const std::string& name) {
            counter_map[name]->Increment();
        }

        void add_gauge(const std::string& name) {
            gauge_map[name].reset(&BuildGauge()
                .Name(name)
                .Register(*registry)
                .Add({})
            );
        }

        void update_gauge(const std::string& name, const double value) {
            gauge_map[name]->Set(value);
        }

        void add_histogram(const std::string& name, const std::vector<double>& keypoints) {
            histogram_map[name].reset(&BuildHistogram()
                    .Name(name)
                    .Register(*registry)
                    .Add({}, keypoints));
        }

        void update_histogram(const std::string& name, const double value) {
            histogram_map[name]->Observe(value);
        }

        virtual ~telemetry_plugin_impl() = default;
    };

    telemetry_plugin::telemetry_plugin() : my(new telemetry_plugin_impl()) {}

    telemetry_plugin::~telemetry_plugin() = default;

    void telemetry_plugin::set_program_options(options_description &, options_description &cfg) {
        cfg.add_options()
                ("telemetry-endpoint", bpo::value<string>()->default_value("8080"),
                 "the endpoint upon which to listen for incoming connections to promethus server")
                ("telemetry-uri", bpo::value<string>()->default_value("/metrics"),
                 "the base uri of the endpoint")
                ("telemetry-threads", bpo::value<size_t>()->default_value(1),
                 "the number of threads to use to process network messages to promethus server");
    }

    void telemetry_plugin::plugin_initialize(const variables_map &options) {
        ilog("Initialize telemetry plugin");
        try {
            my->endpoint = options.at("telemetry-endpoint").as<string>();
            my->uri = options.at("telemetry-uri").as<string>();
            my->threads = options.at("telemetry-threads").as<size_t>();
        }
        FC_LOG_AND_RETHROW();
    }

    void telemetry_plugin::plugin_startup() {
        wlog("Telemetry plugin startup");
        try {
            my->initialize();
            ilog("Telemetry plugin started, started listening endpoint (port) ${endpoint} with uri ${uri}",
                 ("endpoint", my->endpoint)("uri", my->uri));
        }
        FC_LOG_AND_RETHROW();
    }

    void telemetry_plugin::plugin_shutdown() {
        wlog("Telemetry plugin shutdown");
    }

    void telemetry_plugin::add_counter(const std::string& metric_name) {
        my->add_counter(metric_name);
    }

    void telemetry_plugin::update_counter(const std::string& metric_name) {
        my->update_counter(metric_name);
    }

    void telemetry_plugin::add_gauge(const std::string& metric_name) {
        my->add_gauge(metric_name);
    }

    void telemetry_plugin::update_gauge(const std::string& metric_name, const double value) {
        my->update_gauge(metric_name, value);
    }

    void telemetry_plugin::add_histogram(const std::string& metric_name, const vector<double>& keypoints) {
        my->add_histogram(metric_name, keypoints);
    }

    void telemetry_plugin::update_histogram(const std::string& metric_name, const double value) {
        my->update_histogram(metric_name, value);
    }
}

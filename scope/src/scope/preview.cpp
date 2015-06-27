#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>

#include <iostream>

namespace sc = unity::scopes;

using namespace std;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    sc::Result result = PreviewQueryBase::result();

    // Support three different column layouts
    sc::ColumnLayout layout1col(1), layout2col(2), layout3col(3);

    /**
      * Welcome view
      */
    if(result["type"].get_string() == "welcome") {
        layout1col.add_column( { "image", "header" });
        reply->register_layout( { layout1col });

        // Define the header section
        sc::PreviewWidget header("header", "header");
        header.add_attribute_mapping("title", "title");

        // Define the image section
        sc::PreviewWidget image("image", "image");
        image.add_attribute_mapping("source", "art");

        // Push each of the sections
        reply->push( { image, header });
    }
    /**
      * No results
      */
    else if(result["type"].get_string() == "empty") {
        layout1col.add_column( { "image", "header" });
        reply->register_layout( { layout1col });

        // Define the header section
        sc::PreviewWidget header("header", "header");
        header.add_attribute_mapping("title", "title");

        // Define the image section
        sc::PreviewWidget image("image", "image");
        image.add_attribute_mapping("source", "art");

        // Push each of the sections
        reply->push( { image, header });
    }
    /**
      * Result preview
      */
    else if(result["type"].get_string() == "found") {
        // Single column layout
        layout1col.add_column( { "image", "header", "summary", "actions", "expandable" });

        // Two column layout
        layout2col.add_column( { "image", "actions" });
        layout2col.add_column( { "header", "summary", "expandable" });

        // Three cokumn layout
        layout3col.add_column( { "image", "actions" });
        layout3col.add_column( { "header", "summary" });
        layout3col.add_column( { "expandable" });

        // Register the layouts we just created
        reply->register_layout( { layout1col, layout2col, layout3col });

        // Define the header section
        sc::PreviewWidget header("header", "header");
        // It has title and a subtitle properties
        header.add_attribute_mapping("title", "title");
        header.add_attribute_mapping("subtitle", "subtitle");

        // Define the image section
        sc::PreviewWidget image("image", "image");
        // It has a single source property, mapped to the result's art property
        image.add_attribute_mapping("source", "art");

        // Define the summary section
        sc::PreviewWidget description("summary", "text");
        // It has a text property, mapped to the result's description property
        description.add_attribute_mapping("text", "description");

        // Original article
        sc::PreviewWidget expandable("expandable", "expandable");
        expandable.add_attribute_value("title", sc::Variant("Original Article"));

        sc::PreviewWidget original("original", "text");
        original.add_attribute_value("text", result["original"]);

        expandable.add_widget(original);

        expandable.add_attribute_value("collapsed-widgets", sc::Variant("text"));

        // Actions
        sc::PreviewWidget actions("actions", "actions");
        sc::VariantBuilder builder;
        builder.add_tuple({
                              {"id", sc::Variant("open")},
                              {"label", sc::Variant("View")},
                              {"uri", result["link"]}
                          });
        actions.add_attribute_value("actions", builder.end());

        // Push each of the sections
        reply->push( { image, header, description, actions, expandable });
    }
}


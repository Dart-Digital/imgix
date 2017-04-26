/**
 * @file
 * A JavaScript file for carousel.
 */

(function ($, Drupal, window, document, undefined) {

  Drupal.behaviors.imgix = {
    attach: function(context, settings) {
      imgix.fluid({
        fluidClass: 'imgix-style-fluid',
        // Use this to maintain the aspect ratio as defined in the style.
        onChangeParamOverride: function(w, h, params, elem) {
          var orig = new imgix.URL($(elem).data('src')),
            orig_w = orig.getWidth(),
            orig_h = orig.getHeight(),
            fit = orig.getFit();

          // If the w and h have been set, we'll keep that same aspect ratio.
          if (!$.isNumeric(orig_w)) {
            w = '<unset>';
          }

          if (!$.isNumeric(orig_h)) {
            h = '<unset>';
          }

          if ($.isNumeric(orig_h) && $.isNumeric(orig_w) && orig_h > 1 && orig_w > 1) {
            // They've specified the original dimensions in px, so we try to
            // keep that aspect ratio.
            var ratio = orig_w / orig_h;
            h = w / ratio;
          }

          var config = {};
          config.w = w;
          config.h = h;
          config.fit = fit;

          return config;
        }
      });
    }
  };

})(jQuery, Drupal, this, this.document);

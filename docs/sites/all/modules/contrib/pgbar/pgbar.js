'use strict';

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

// Generated by CoffeeScript 2.3.1
var $, PgbarItem, formatNumber;

$ = jQuery;

if (Drupal.formatNumber != null) {
  formatNumber = function formatNumber(now) {
    return Drupal.formatNumber(now, 0);
  };
} else {
  formatNumber = function formatNumber(now) {
    var num, rest, zeros;
    num = '';
    // Add thousand separators to the number
    zeros = 0;
    now = Math.round(now);
    if (now === 0) {
      return '0';
    }
    while (now > 0) {
      while (zeros > 0) {
        num = '0' + num;
        zeros -= 1;
      }
      rest = now % 1000;
      zeros = 3 - rest.toString().length;
      num = rest + ',' + num;
      now = (now - rest) / 1000;
    }
    // cut last thousand separator from output.
    return num.slice(0, num.length - 1);
  };
}

PgbarItem = function () {
  function PgbarItem(settings1, wrapper) {
    var _this = this;

    _classCallCheck(this, PgbarItem);

    this.settings = settings1;
    this.wrapper = wrapper;
    this.current = 0;
    this.counter = $('.pgbar-counter', wrapper);
    this.bars = $('.pgbar-current', wrapper);
    if (this.settings.extractor) {
      this.extractor = this.settings.extractor;
    } else {
      this.extractor = function (data) {
        return parseInt(data.pgbar[_this.settings.field_name][_this.settings.delta]);
      };
    }
  }

  _createClass(PgbarItem, [{
    key: 'poll',
    value: function poll() {
      var _this2 = this;

      var callback, registry;
      registry = Drupal.behaviors.polling.registry;
      callback = function callback(data) {
        var to_abs;
        to_abs = _this2.extractor(data);
        if (to_abs !== _this2.current) {
          _this2.animate(to_abs);
        }
      };
      return registry.registerUrl(this.settings.pollingURL, this.settings.id, callback);
    }
  }, {
    key: 'animate',
    value: function animate(to_abs) {
      var _this3 = this;

      var from_abs = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : this.current;

      var diff, duration, from, resetCounters, target, to;
      target = this.settings.target;
      if (this.settings.inverted) {
        from = 1 - from_abs / target;
        to = 1 - to_abs / target;
        diff = from - to;
      } else {
        from = from_abs / target;
        to = to_abs / target;
        diff = to - from;
      }
      this.counter.html(formatNumber(from_abs));
      duration = 500 + 1000 * diff;
      resetCounters = function resetCounters(num, fx) {
        return _this3.counter.html(formatNumber(num));
      };
      if (this.settings.vertical) {
        this.bars.height(from * 100 + '%');
        this.bars.animate({
          height: to * 100 + '%'
        }, {
          duration: duration
        });
      } else {
        this.bars.width(from * 100 + '%');
        this.bars.animate({
          width: to * 100 + '%'
        }, {
          duration: duration
        });
      }
      this.wrapper.animate({
        val: to_abs
      }, {
        duration: duration,
        step: resetCounters
      });
      return this.current = to_abs;
    }
  }, {
    key: 'animateInitially',
    value: function animateInitially() {
      var _this4 = this;

      var animation;
      animation = function animation() {
        return _this4.animate(_this4.settings.current);
      };
      return window.setTimeout(animation, 2000);
    }
  }]);

  return PgbarItem;
}();

PgbarItem.fromElement = function ($element) {
  var id, settings;
  id = $element.attr('id');
  settings = Drupal.settings.pgbar[id];
  settings['id'] = id;
  settings['inverted'] = $element.data('pgbarInverted');
  settings['vertical'] = $element.data('pgbarDirection') === 'vertical';
  return new PgbarItem(settings, $element);
};

Drupal.behaviors.pgbar = {};

Drupal.behaviors.pgbar.attach = function (context, settings) {
  return $('.pgbar-wrapper[id]', context).each(function () {
    var item;
    item = PgbarItem.fromElement($(this));
    if (item.settings['autostart']) {
      item.animateInitially();
      return item.poll();
    }
  });
};
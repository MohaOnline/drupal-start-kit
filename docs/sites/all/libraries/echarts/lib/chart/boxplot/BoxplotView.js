var zrUtil = require("zrender/lib/core/util");

var ChartView = require("../../view/Chart");

var graphic = require("../../util/graphic");

var _whiskerBoxCommon = require("../helper/whiskerBoxCommon");

var viewMixin = _whiskerBoxCommon.viewMixin;
var BoxplotView = ChartView.extend({
  type: 'boxplot',
  getStyleUpdater: function () {
    return updateStyle;
  },
  dispose: zrUtil.noop
});
zrUtil.mixin(BoxplotView, viewMixin, true); // Update common properties

var normalStyleAccessPath = ['itemStyle'];
var emphasisStyleAccessPath = ['emphasis', 'itemStyle'];

function updateStyle(itemGroup, data, idx) {
  var itemModel = data.getItemModel(idx);
  var normalItemStyleModel = itemModel.getModel(normalStyleAccessPath);
  var borderColor = data.getItemVisual(idx, 'color'); // Exclude borderColor.

  var itemStyle = normalItemStyleModel.getItemStyle(['borderColor']);
  var whiskerEl = itemGroup.childAt(itemGroup.whiskerIndex);
  whiskerEl.style.set(itemStyle);
  whiskerEl.style.stroke = borderColor;
  whiskerEl.dirty();
  var bodyEl = itemGroup.childAt(itemGroup.bodyIndex);
  bodyEl.style.set(itemStyle);
  bodyEl.style.stroke = borderColor;
  bodyEl.dirty();
  var hoverStyle = itemModel.getModel(emphasisStyleAccessPath).getItemStyle();
  graphic.setHoverStyle(itemGroup, hoverStyle);
}

var _default = BoxplotView;
module.exports = _default;
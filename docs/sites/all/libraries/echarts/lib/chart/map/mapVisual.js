function _default(ecModel) {
  ecModel.eachSeriesByType('map', function (seriesModel) {
    var colorList = seriesModel.get('color');
    var itemStyleModel = seriesModel.getModel('itemStyle');
    var areaColor = itemStyleModel.get('areaColor');
    var color = itemStyleModel.get('color') || colorList[seriesModel.seriesIndex % colorList.length];
    seriesModel.getData().setVisual({
      'areaColor': areaColor,
      'color': color
    });
  });
}

module.exports = _default;
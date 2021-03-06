<?php

/**
 * @copyright Copyright (c) 2018 Palantir.net
 */

/**
 * Class SearchApiFederatedSolrRemap
 * Provides a Search API index data alteration that remaps property names for indexed items.
 */
class SearchApiFederatedSolrRemap extends SearchApiAbstractAlterCallback {

  /**
   * {@inheritdoc}
   */
  public function propertyInfo() {
    if (is_array($this->options['properties'])) {
      return $this->options['properties'];
    }

    return [];
  }


  /**
   * {@inheritdoc}
   */
  public function alterItems(array &$items) {
    foreach ($items as &$item) {
      foreach ($this->options['remap'] as $destination => $source) {
        if ($source && isset($item->{$source})) {
          $item->{$destination} = $item->{$source};
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function configurationForm() {
    $form['remap'] = [
      '#type' => 'fieldset',
      '#title' => t('Remap properties'),
    ];
    foreach ($this->federatedFields() as $k => $field) {
      $form['remap'][$k] = [
        '#type' => 'select',
        '#title' => $field['name'],
        '#options' => $this->indexFieldOptions(),
        '#default_value' => isset($this->options['remap'][$k]) ? $this->options['remap'][$k] : '',
      ];
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   *
   * Not all of the source field information is available when the propertyInfo()
   * method is called, so we set up the properties here and store them in the
   * plugin options.
   */
  public function configurationFormSubmit(array $form, array &$values, array &$form_state) {
    $properties = [];

    $fields = $this->index->getFields(FALSE);
    foreach (array_filter($values['remap']) as $destination => $source) {
      $properties[$destination] = [
        'label' => t('@field (remapped from @key)', ['@field' => $fields[$source]['name'], '@key' => $source]),
        'description' => $fields[$source]['description'],
        'type' => $fields[$source]['type'],
      ];
    }

    $values['properties'] = $properties;

    return parent::configurationFormSubmit($form, $values, $form_state); // TODO: Change the autogenerated stub
  }

  /**
   * This is the list of possible destination fields.
   *
   * @return array
   *
   * @see docs/federated_schema.md
   */
  protected function federatedFields() {
    return [
      'federated_title' => [
        'name' => t('Federated Title'),
        'description' => '',
        'type' => 'string'
      ],
      'federated_date' => [
        'name' => t('Federated Date'),
        'description' => '',
        'type' => 'date'
      ],
      'federated_type' => [
        'name' => t('Federated Type'),
        'description' => '',
        'type' => 'string'
      ],
      'federated_terms' => [
        'name' => t('Federated Terms'),
        'description' => '',
        'type' => 'string'
      ],
      'federated_image' => [
        'name' => t('Federated Image'),
        'description' => '',
        'type' => 'uri'
      ],
      'rendered_item' => [
        'name' => t('Rendered Item'),
        'description' => '',
        'type' => 'text',
      ],
    ];
  }

  /**
   * Get a form options array containing all of the fields that can be remapped.
   *
   * @return array
   */
  protected function indexFieldOptions() {
    $options = array_diff_key($this->index->getFields(FALSE), $this->federatedFields());

    array_walk($options, function (&$item, $key) {
      $item = t('@name (@machine_name)', ['@name' => $item['name'], '@machine_name' => $key]);
    });

    return ['- ' . t('None') . ' -'] + $options;
  }

}

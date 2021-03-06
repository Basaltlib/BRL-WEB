<?php

use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\Tags;

/**
 * Clean away twig debugging output, it breaks some stuff.
 * @param $variable
 * @return mixed
 */
function _at_core_cleanup_twig_debug_output($variable) {
  $output = preg_replace('/[ \t]+/', ' ', preg_replace('/<!--(.|\s)*?-->/', '', $variable));
  return preg_replace(['/\r/', '/\n/'], '', $output);
}

/**
 * Return values for the breadcrumb title placeholder.
 * @param $page_title
 * @return array
 */
function _at_core_breadcrumb_title_placeholder($page_title) {
  $request = \Drupal::request();
  $route_match = \Drupal::routeMatch();
  $title = \Drupal::service('title_resolver')->getTitle($request, $route_match->getRouteObject());
  $array = [
    '#theme' => 'page_title__breadcrumb',
    '#title' => $title,
  ];
  return $array;
}

/**
 * Helper function for layout plugin attributes.
 * @param $variables
 * @return mixed
 */
function _at_core_layout_plugin_attributes($variables) {
  $html_element = 'div';
  $classes = [];
  $role = NULL;
  $bundle = NULL;
  $entity_type = NULL;
  $view_mode = NULL;
  $id = NULL;;

  // Entity and bundle.
  if (isset($variables['content']['#entity_type'])) {
    $entity_type = $variables['content']['#entity_type'];
    $bundle = $variables['content']['#bundle'];
    $view_mode = $variables['content']['#view_mode'];
    $id = $variables['content']['#'. $entity_type]->id();

    $entity_classes = !empty($variables['content']['#ds_configuration']['layout']['entity_classes']) ? $variables['content']['#ds_configuration']['layout']['entity_classes'] : '';
    if ($entity_classes !== 'no_classes') {
      $classes[] = $entity_type;
      $classes[] = $view_mode ? Html::cleanCssIdentifier($entity_type . '--view-mode-' . $view_mode) : '';
      $classes[] = $bundle ? Html::cleanCssIdentifier($entity_type . '--type-' . $bundle) : '';
      $classes[] = $id ? Html::cleanCssIdentifier($entity_type . '--id-' . $id) : '';
    }

    if (in_array($entity_type, ['node', 'comment'])) {
      $html_element = 'article';
      $role = 'article';
    }
    if ($entity_type == 'comment') {
      $classes[] = 'js-comment';
      $classes[] = $variables['content']['#comment']->getStatus() != 1 ? 'unpublished' : '';
      if ($variables['content']['#comment']->getCommentedEntityTypeId() !== 'block_content') {
        $comment_owner = $variables['content']['#comment']->getOwnerId();
        $commented_entity_owner = $variables['content']['#comment']->getCommentedEntity()->getOwnerId();
        if ($comment_owner === $commented_entity_owner) {
          $classes[] = 'by-' . $variables['content']['#comment']->getCommentedEntity()->getEntityTypeId() . '-author';
        }
      }
    }

    // Extension settings.
    if ($variables['theme']['extensions']['is_enabled'] === TRUE) {
      if ($variables['theme']['shortcodes']['is_enabled'] === TRUE) {
        $shortcodes_config = $variables['theme'][$variables['theme']['shortcodes']['config']];
        if (isset($variables['content'])) {

          //kint($shortcodes_config);

          $entity_type_bundle_setting = $entity_type . 'type_classes_' . $bundle;
          if (isset($shortcodes_config[$entity_type_bundle_setting]) && !empty($shortcodes_config[$entity_type_bundle_setting])) {
            $shortcodes = Tags::explode($shortcodes_config[$entity_type_bundle_setting]);
            foreach ($shortcodes as $class) {
              $classes[] = Html::cleanCssIdentifier($class);
            }
          }
        }
      }
    }
  }

  // Panels
  if (isset($variables['content']['#page_variant'])) {
    $classes[] = 'panels-page';
    $classes[] = 'panels-page--' . Html::cleanCssIdentifier($variables['content']['#page_variant']->get('page'));
  }

  // Global classes
  $layout = 'none';
  // Layout plugin contrib module, Drupal core 8.2.x and below.
  if (isset($variables['layout']['id'])) {
    $layout = $variables['layout']['id'];
  }
  // Layout discovery, Drupal core 8.3.x and up.
  elseif (isset($variables['content']['#layout'])) {
    $layout = $variables['content']['#layout']->id();
  }

  $classes[] = 'atl';
  $classes[] = 'atl--' . Html::cleanCssIdentifier($layout);

  $attributes['classes'] = $classes;
  $attributes['html_element'] = $html_element;
  if (!empty($role)) {
    $attributes['role'] = $role;
  }

  return $attributes;
}
